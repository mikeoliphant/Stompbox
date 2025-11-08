#include <math.h>
#include <algorithm>

#include "GraphicEqualizer.h"

GraphicEqualizer::GraphicEqualizer(int numBands, float* frequencies, float q)
{
	Name = "EQ";

	this->numBands = numBands;

	volume = 0;

	NumParameters = numBands + 1;
	CreateParameters(NumParameters);

	peakingFilters = new PeakingFilter*[numBands];

	for (int band = 0; band < numBands; band++)
	{
		float freq = frequencies[band];

		peakingFilters[band] = new PeakingFilter(freq, q);

		char name[80];

		if (freq < 1000)
		{
			snprintf(name, 80, "%d", (int)freq);
		}
		else
		{
			snprintf(name, 80, "%.1fk", freq / 1000);
		}

		Parameters[band].Name = name;

		Parameters[band].MinValue = -15;
		Parameters[band].MaxValue = 15;
		Parameters[band].SourceVariable = peakingFilters[band]->GetParameter(PEAKINGFILTER_LEVEL)->SourceVariable;
		Parameters[band].DefaultValue = 0;
		Parameters[band].DisplayFormat = "{0:0.0}dB";
		Parameters[band].ParameterType = PARAMETER_TYPE_VSLIDER;
	}

	Parameters[numBands].Name = "Vol";
	Parameters[numBands].MinValue = -15;
	Parameters[numBands].MaxValue = 15;
	Parameters[numBands].SourceVariable = &volume;
	Parameters[numBands].DefaultValue = volume;
	Parameters[numBands].ParameterType = PARAMETER_TYPE_VSLIDER;
}

GraphicEqualizer::~GraphicEqualizer()
{
	if (peakingFilters != nullptr)
	{
		for (int band = 0; band < numBands; band++)
		{
			//delete Parameters[band].Name;
			delete[] peakingFilters[band];
		}

		delete[] peakingFilters;
	}
}

void GraphicEqualizer::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	linearGain = (float)pow(10.0, (0.05 * volume));

	instanceConstants(samplingFreq);
	instanceClear();
}

void GraphicEqualizer::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	for (int band = 0; band < numBands; band++)
	{
		peakingFilters[band]->instanceConstants(samplingFreq);
	}
}

void GraphicEqualizer::instanceClear() {
	for (int band = 0; band < numBands; band++)
	{
		peakingFilters[band]->instanceClear();
	}
}

void GraphicEqualizer::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	for (int band = 0; band < numBands; band++)
	{
		peakingFilters[band]->compute(count, input0, input0);
	}

	float desiredGain = (float)pow(10.0, (0.05 * volume));

	for (int i = 0; i < count; i++)
	{
		linearGain = (linearGain * .99f) + (desiredGain * .01f);

		output0[i] = input0[i] * linearGain;
	}
}

