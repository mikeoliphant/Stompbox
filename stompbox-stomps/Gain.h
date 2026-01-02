#pragma once

#include "StompBox.h"

class Gain : public StompBox
{
private:
	float gain = 0;
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
