
#include <iostream>
#include <stdio.h>
#include <stdint.h>

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
	char text[10] = { '0','1','2','3','4','5','6','7','8','9' };
	float rightX;
	float rightY;
	float leftX;
	float leftY;
	bool left;
	bool right;
	const uint16_t footer = 0x0FF0;
};

struct ResponsePayload
{
	const uint16_t header = 0xF00F;
	bool status;
	const uint16_t footer = 0x0FF0;
};

int main(void)
{
	std::cout << "initializing wiringPi\n";
	//int wiringHandle = wiringPiSetupSys();
	//printf("%x \n", wiringHandle);

	std::cout << "setting pin modes\n";
	//pinMode(0, INPUT);
	//pinMode(2, INPUT);

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
		requestPayload.rightX = 1 - scale * inRightX;
		requestPayload.rightY = 1 - scale * inRightY;
		requestPayload.leftX = 1 - scale * inLeftX;
		requestPayload.leftY = 1 - scale * inLeftY;
		requestPayload.left = bcm2835_gpio_lev(RPI_V2_GPIO_P1_11);
		requestPayload.right = bcm2835_gpio_lev(RPI_V2_GPIO_P1_13);

		radio.stopListening();
		radio.write(&requestPayload, sizeof(RequestPayload));
		radio.startListening();


		std::cout << "\x1B[2J\x1B[H";
		printf("1 %f %X\n", requestPayload.rightX, inRightX);
		printf("1 %f %X\n", requestPayload.rightY, inRightY);
		printf("2 %f %X\n", requestPayload.leftX, inLeftX);
		printf("3 %f %X\n", requestPayload.leftY, inLeftY);
		printf("A %s \n", requestPayload.left ? "Down" : "Up");
		printf("B %s \n", requestPayload.right ? "Down" : "Up");
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