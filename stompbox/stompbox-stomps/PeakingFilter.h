#pragma once

#include "StompBox.h"

enum
{
	PEAKINGFILTER_LEVEL,
	PEAKINGFILTER_NUMPARAMETERS
};

class PeakingFilter : public StompBox
{
private:
	int fSamplingFreq;

	double freq;
	double q;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	FAUSTFLOAT fVslider0;
	double fRec1[2];
	double fConst5;
	double fRec0[3];

public:
	PeakingFilter(double freq, double q);
	virtual ~PeakingFilter() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
