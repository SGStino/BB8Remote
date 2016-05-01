#pragma once
#include "ADS1115Config.h"
#include <wiringPiI2C.h>
#include <wiringPi.h>
#define REG_CONVERSION 0x00
#define REG_CONFIG 0x01

class ADS1115
{
private:
	int deviceHandle;

	void readConfig();
	void writeConfig();
	void waitForData();
	unsigned short readLastValue();
public:
	ADS1115Config config;

	unsigned short ReadValue(AdcMux mux);
	float ReadMilliVolts(AdcMux mux);
	void Reset();
	ADS1115(int device);
	~ADS1115();
};

