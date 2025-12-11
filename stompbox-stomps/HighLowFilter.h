#pragma once

#include "StompBox.h"

enum
{
	HIGHLOWFILTER_LOW_FREQUENCY,
	HIGHLOWFILTER_HIGH_FREQUENCY,
	HIGHLOWFILTER_NUMPARAMETERS
};

class HighLowFilter : public StompBox
{
private:
	int fSampleRate;

	float lowFreq = 1;
	float highFreq = 20000;

	double fConst0;
	double fConst1;
	double fConst2;
	double fRec0[2];
	double fConst3;
	double fRec2[2];
	double fRec3[3];
	double fRec1[3];

public:
	HighLowFilter();
	virtual ~HighLowFilter() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
