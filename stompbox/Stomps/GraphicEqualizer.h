#pragma once

#include "StompBox.h"
#include "PeakingFilter.h"

class GraphicEqualizer : public StompBox
{
private:
	int fSamplingFreq;

	float volume;
	int numBands;
	PeakingFilter** peakingFilters = nullptr;
	float linearGain;

public:
	GraphicEqualizer(int numBands, float* frequencies, float q);
	virtual ~GraphicEqualizer();
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
