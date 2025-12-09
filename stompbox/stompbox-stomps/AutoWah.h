#pragma once

#include "StompBox.h"

enum
{
	AUTOWAH_LEVEL,
	AUTOWAH_NUMPARAMETERS
};

class AutoWah : public StompBox
{
private:
	int fSamplingFreq;

	FAUSTFLOAT fVslider0;
	double fConst0;
	double fConst1;
	double fConst2;
	double fRec2[2];
	double fRec1[2];
	double fConst3;
	double fRec3[2];
	double fConst4;
	double fRec4[2];
	double fRec0[3];

public:
	AutoWah();
	virtual ~AutoWah() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, float* input, float* output);
};
