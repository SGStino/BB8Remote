#include "ADS1115.h"
#include <stdio.h>
#include <iostream>
void ADS1115::readConfig()
{
	config.value = wiringPiI2CReadReg16(deviceHandle, REG_CONFIG);

}

void ADS1115::writeConfig()
{
	auto config = this->config.value;
	config = ((config >> 8) & 0x00FF) | ((config << 8) & 0xFF00);
	wiringPiI2CWriteReg16(deviceHandle, REG_CONFIG, config);
}

unsigned short ADS1115::readLastValue()
{
	//wiringPiI2CWrite(deviceHandle, REG_CONVERSION);
	//auto b0 = wiringPiI2CRead(deviceHandle);
	//auto b1 = wiringPiI2CRead(deviceHandle);

	//  return ((b0 << 8) & 0xFF00) | (b1 & 0xFF);
	  /*
	if (value > 0x7FFF) {
		return (value - 0xFFFF);
	}
	else {
		return value;
	}
	*/


	auto value = wiringPiI2CReadReg16(deviceHandle, REG_CONVERSION);
	value = ((value >> 8) & 0x00FF) | ((value << 8) & 0xFF00); // flip endianness
	return value;
}

void ADS1115::waitForData()
{
	delay(2);
}

unsigned short ADS1115::ReadValue(AdcMux mux)
{
	config.SetInputMultiplexer(mux);
	config.SetSingleShot(true);
	config.SetActive(true);
	this->writeConfig();
	waitForData();
	return readLastValue();
}

float ADS1115::ReadMilliVolts(AdcMux mux)
{
	auto value = ReadValue(mux);
	switch (config.GetGainAmplification())
	{
	case AdcGain::Gain6V144:
		return value * 6.144f / 0xFFFF;
	case AdcGain::Gain4V096:
		return value * 4.096f / 0xFFFF;
	case AdcGain::Gain2V048:
		return value * 2.048f / 0xFFFF;
	case AdcGain::Gain1V024:
		return value * 1.024f / 0xFFFF;
	case AdcGain::Gain0V512:
		return value * 0.512f / 0xFFFF;

	case AdcGain::Gain0V256:
	case AdcGain::Gain0V256b:
	case AdcGain::Gain0V256c:
		return value * 0.256f / 0xFFFF;
	}
	return 0.0f;
}

void ADS1115::Reset()
{
	this->writeConfig();
}

ADS1115::ADS1115(const int device)
{
	deviceHandle = wiringPiI2CSetup(device);
	Reset();
}


ADS1115::~ADS1115()
{
}
