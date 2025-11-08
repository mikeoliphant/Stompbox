#pragma once

#include "StompBox.h"

enum
{
	GAIN_GAIN,
	GAIN_LEVEL,
	GAIN_NUMPARAMETERS
};

class Gain : public StompBox
{
private:
	float gain;
	float minGain;
	float maxGain;
	float linearGain;
	float currentLevel = 0;

public:
	Gain(float initialGain, float minGain, float maxGain);
	~Gain() {}
	virtual void init(int samplingFreq);
	double GetLevel();

	virtual void compute(int count, float* input, float* output);
};
