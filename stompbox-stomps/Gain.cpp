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

	auto& gainParam = AddParameter();
	gainParam.Name = "Gain";
	gainParam.SourceVariable = &gain;
	gainParam.MinValue = minGain;
	gainParam.MaxValue = maxGain;
	gainParam.DefaultValue = initialGain;
	gainParam.DisplayFormat = "{0:0.0}dB";
	gainParam.Description = "Gain strength in dB";

	auto& levelParam = AddParameter();
	levelParam.Name = "Level";
	levelParam.SourceVariable = &currentLevel;
	levelParam.IsOutput = true;
	levelParam.MinValue = 0;
	levelParam.MaxValue = 1;
	levelParam.DefaultValue = currentLevel;
	levelParam.Description = "Current Level";
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