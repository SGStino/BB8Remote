
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <cmath>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <RF24/RF24.h>

#define I2C_ADC_ADDR  0x48

#include "ADS1115Config.h"
#include "ADS1115.h"


RF24 radio(RPI_V2_GPIO_P1_15, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

struct RequestPayload
{
	const uint16_t header = 0xF00F;
	uint8_t directions;
	uint8_t speeds[6];
	uint8_t reserved;
	const uint16_t footer = 0x0FF0;
};

struct ResponsePayload
{
	const uint16_t header = 0xF00F;
	bool status;
	const uint16_t footer = 0x0FF0;
};
float clip(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}

const float scaleX = -1.0f / 2.0f;
const float scaleY = 1.73205080757f / 2.0f;

int main(void)
{
	std::cout << "initializing wiringPi\n";
	//int wiringHandle = wiringPiSetupSys();
	//printf("%x \n", wiringHandle); 
	std::cout << "init" << bcm2835_init() << "\n";

	std::cout << "setting pin modes\n";
	//pinMode(0, INPUT);
	//pinMode(2, INPUT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_11, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13, BCM2835_GPIO_FSEL_INPT);

	std::cout << "setting pin pullups\n";
	//pullUpDnControl(0, PUD_UP);
	//pullUpDnControl(2, PUD_UP);

	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_11, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_set_pud(RPI_V2_GPIO_P1_13, BCM2835_GPIO_PUD_UP);

	std::cout << "initializing ADS1115\n";
	auto ads1115 = new ADS1115(I2C_ADC_ADDR);
	ads1115->config.SetGainAmplification(AdcGain::Gain2V048);
	ads1115->config.SetDataRate(AdcDataRate::SPS3300);
	float scale = 2 * 1.0f / 0x7FFF;


	std::cout << "initializing RF24 radio\n";
	radio.begin();
	radio.enableDynamicPayloads();
	radio.setRetries(5, 15);
	radio.printDetails();

	std::cout << "opening radio pioes\n";
	radio.openWritingPipe(pipes[0]);
	radio.openReadingPipe(1, pipes[1]);
	auto requestPayload = RequestPayload();
	auto responsePayload = ResponsePayload();
	std::cout << "starting loop\n";
	while (true)
	{
		auto inRightX = ads1115->ReadValue(AdcMux::AINP0NGND);
		auto inRightY = ads1115->ReadValue(AdcMux::AINP1NGND);
		auto inLeftX = ads1115->ReadValue(AdcMux::AINP2NGND);
		auto inLeftY = ads1115->ReadValue(AdcMux::AINP3NGND);

		

		auto rightX = clip(1 - scale * inRightX, -1, 1);
		auto rightY = clip(1 - scale * inRightY, -1, 1);

		auto powerRight = std::sqrt(rightX * rightX + rightY * rightY);

		rightX *= powerRight;
		rightY *= powerRight;


		auto leftX = clip(1 - scale * inLeftX, -1, 1);
		auto leftY = clip(1 - scale * inLeftY, -1, 1);


		auto powerLeft = std::sqrt(leftX * leftX + leftY * leftY);

		leftX *= powerLeft;
		leftY *= powerLeft;


		auto left = bcm2835_gpio_lev(RPI_V2_GPIO_P1_11);
		auto right = bcm2835_gpio_lev(RPI_V2_GPIO_P1_13);


		std::cout << "\x1B[2J\x1B[H";

		float rotation = 0;
		float headRotation = (left ? 1 : 0) + (right ? -1 : 0);
		float motors[6];

		motors[0] = (scaleX * rightX) - (scaleY * rightY) + rotation;
		motors[1] = (scaleX * rightX) + (scaleY * rightY) + rotation;
		motors[2] = rightX + rotation;

		motors[3] = headRotation;
		motors[4] = leftX;
		motors[5] = leftY;

		requestPayload.directions = 0; 
		for (int i = 0; i < 6; i++)
		{
			if (motors[i] < 0)
				requestPayload.directions |= 1 << i;
			requestPayload.speeds[i] = clip(abs(motors[i] * 255.0f),0,255.0f);
			printf("%x %f %X\n",i, motors[i], requestPayload.speeds[i]);
		}
		printf("%x\n", requestPayload.directions);


		radio.stopListening();
		radio.write(&requestPayload, sizeof(RequestPayload));
		radio.startListening();


		printf("1 %f %X\n", rightX, inRightX);
		printf("1 %f %X\n", rightY, inRightY);
		printf("2 %f %X\n", leftX, inLeftX);
		printf("3 %f %X\n", leftY, inLeftY);
		printf("A %s \n", left ? "Down" : "Up");
		printf("B %s \n", right ? "Down" : "Up");
		std::cout << "done\n";

		unsigned long started_waiting_at = millis();
		bool timeout = false;
		while (!radio.available() && !timeout)
			if (millis() - started_waiting_at > 500)
				timeout = true;

		std::cout << "available\n";
		if (timeout)
			std::cout << "TIMEOUT\n";
		else
		{
			uint8_t responseSize = radio.getDynamicPayloadSize();
			if (responseSize != sizeof(ResponsePayload))
				std::cout << "UNKNOWN PAYLOAD SIZE";
			else {
				radio.read(&responsePayload, responseSize);
				printf("R: %s", responsePayload.status ? "Success" : "Fail");
			}
		}

	}
	delete ads1115;
	return 0;
}