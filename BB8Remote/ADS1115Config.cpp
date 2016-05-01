#include "ADS1115Config.h"

void ADS1115Config::SetCompQueue(AdcCompQueue queue)
{
	value = (value & 0XFFFC) | (unsigned short)queue;
}

AdcCompQueue ADS1115Config::GetCompQueue()
{
	return (AdcCompQueue)(value & 0x3);
}

void ADS1115Config::SetLatchComparator(bool value)
{
	setConfigBit(1 << 2, value);
}

bool ADS1115Config::GetLatchComparator()
{
	return getConfigBit(1 << 2);
}

void ADS1115Config::SetInvertComparator(bool value)
{
	setConfigBit(1 << 3, value);
}

bool ADS1115Config::GetInvertComparator()
{
	return getConfigBit(1 << 3);
}

void ADS1115Config::SetUseComparatorWindow(bool value)
{
	setConfigBit(1 << 4, value);
}

bool ADS1115Config::GetUseComparatorWindow()
{
	return getConfigBit(1 << 4);
}

void ADS1115Config::SetDataRate(AdcDataRate rate)
{
	value = (value & ~(0x7 << 5)) | ((unsigned short)rate << 5);
}

AdcDataRate ADS1115Config::GetDataRate()
{
	return (AdcDataRate)((value >> 5) & 0x7);
}

void ADS1115Config::SetSingleShot(bool value)
{
	setConfigBit(1 << 8, value);
}

bool ADS1115Config::GetSingleShot()
{
	return getConfigBit(1 << 8);
}

void ADS1115Config::SetGainAmplification(AdcGain gain)
{
	value = (value & ~(0x7 << 9)) | ((unsigned short)gain << 9);
}

AdcGain ADS1115Config::GetGainAmplification()
{
	return (AdcGain)((value >> 9) & 0x7);
}

void ADS1115Config::SetInputMultiplexer(AdcMux mux)
{
	value = (value & ~(0x7 << 12)) | ((unsigned short)mux << 12);
}

AdcMux ADS1115Config::GetInputMultiplexer()
{
	return (AdcMux)((value >> 12) & 0x7);
}

void ADS1115Config::SetActive(bool value)
{
	setConfigBit(1 << 15, value);
}

bool ADS1115Config::GetActive()
{
	return getConfigBit(1 << 15);
}

inline void ADS1115Config::setConfigBit(unsigned short bit, bool set)
{
	if (set)
		value |= bit;
	else
		value &= ~bit;
}

inline bool ADS1115Config::getConfigBit(unsigned short bit)
{
	return (value & bit) == bit;
}
