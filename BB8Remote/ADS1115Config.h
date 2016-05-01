#pragma once
 
enum AdcCompQueue
{
	One = 0,
	Two = 1,
	Four = 2,
	Disabled = 3
};
enum AdcDataRate
{
	SPS128 = 0,
	SPS250 = 1,
	SPS490 = 2,
	SPS920 = 3,
	SPS1600 = 4,
	SPS2400 = 5,
	SPS3300 = 6,
	SPS3300B = 7
};
enum AdcGain
{
	Gain6V144 = 0,
	Gain4V096 = 1,
	Gain2V048 = 2,
	Gain1V024 = 3,
	Gain0V512 = 4,
	Gain0V256 = 5,
	Gain0V256b = 6,
	Gain0V256c = 7
};
enum AdcMux
{
	AINP0N1 = 0,
	AINP0N3 = 1,
	AINP1N3 = 2,
	AINP2N3 = 3,
	AINP0NGND = 4,
	AINP1NGND = 5,
	AINP2NGND = 6,
	AINP3NGND = 7
};
struct ADS1115Config
{ 
public:
	unsigned short value = 0x8385;

	 void SetCompQueue(AdcCompQueue queue);
	 AdcCompQueue GetCompQueue();

	 void SetLatchComparator(bool value);
	 bool GetLatchComparator();

	 void SetInvertComparator(bool value);
	 bool GetInvertComparator();

	 void SetUseComparatorWindow(bool value);
	 bool GetUseComparatorWindow();

	 void SetDataRate(AdcDataRate rate);
	 AdcDataRate GetDataRate();

	void SetSingleShot(bool value);
	bool GetSingleShot();

	void SetGainAmplification(AdcGain gain);
	AdcGain GetGainAmplification();

	void SetInputMultiplexer(AdcMux mux);
	AdcMux GetInputMultiplexer();


	void SetActive(bool value);
	bool GetActive();


private: 
	inline void setConfigBit(const unsigned short bit, bool value);
	inline bool getConfigBit(const unsigned short bit);
};

