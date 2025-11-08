#include <math.h>
#include <algorithm>
#include "Gain.h"

Gain::Gain(float initialGain, float minGain, float maxGain)
{
	Name = "Gain";
	Description = "Simple volume/gain effect";

	gain = initialGain;
	this->minGain = minGain;
	this->maxGain = maxGain;

	NumParameters = GAIN_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[GAIN_GAIN].Name = "Gain";
	Parameters[GAIN_GAIN].SourceVariable = &gain;
	Parameters[GAIN_GAIN].MinValue = minGain;
	Parameters[GAIN_GAIN].MaxValue = maxGain;
	Parameters[GAIN_GAIN].DefaultValue = initialGain;
	Parameters[GAIN_GAIN].DisplayFormat = "{0:0.0}dB";
	Parameters[GAIN_GAIN].Description = "Gain strength in dB";

	Parameters[GAIN_LEVEL].Name = "Level";
	Parameters[GAIN_LEVEL].SourceVariable = &currentLevel;
	Parameters[GAIN_LEVEL].IsOutput = true;
	Parameters[GAIN_LEVEL].MinValue = 0;
	Parameters[GAIN_LEVEL].MaxValue = 1;
	Parameters[GAIN_LEVEL].DefaultValue = currentLevel;
	Parameters[GAIN_LEVEL].Description = "Current Level";
}

double Gain::GetLevel()
{
	return currentLevel;
}

void Gain::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	linearGain = (float)pow(10.0, (0.05 * gain));

	instanceConstants(samplingFreq);
	instanceClear();
}

void Gain::compute(int count, float* input0, float* output0)
{
	float desiredGain = (float)pow(10.0, (0.05 * gain));

	float level = 0;

	for (int i = 0; i < count; i++)
	{
		linearGain = (linearGain * .99f) + (desiredGain * .01f);

		output0[i] = input0[i] * linearGain;

		level = std::max(level, abs(output0[i]));
	}

	currentLevel -= ((float)count / (float)samplingFreq) * 10 * currentLevel;

	if (currentLevel < 0)
		currentLevel = 0;

	currentLevel = std::max(currentLevel, level);
}