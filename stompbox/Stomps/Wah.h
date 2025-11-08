#pragma once

#include "StompBox.h"

enum
{
	WAH_LEVEL,
	WAH_NUMPARAMETERS
};

class Wah : public StompBox
{
private:
	int fSamplingFreq;

	FAUSTFLOAT fVslider0;
	double fRec1[2];
	double fConst0;
	double fConst1;
	double fConst2;
	double fRec2[2];
	double fRec3[2];
	double fRec0[3];

public:
	Wah();
	virtual ~Wah() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, float* input, float* output);
};
