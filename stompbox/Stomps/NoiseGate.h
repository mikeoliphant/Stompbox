#pragma once

#include "StompBox.h"

enum
{
	NOISEGATE_THRESHOLD,
	NOISEGATE_ATTACK,
	NOISEGATE_HOLD,
	NOISEGATE_RELEASE,
	NOISEGATE_NUMPARAMETERS
};

class NoiseGate : public StompBox
{
private:
	int fSamplingFreq;

	float threshold;
	float attackMS;
	float holdMS;
	float releaseMS;

	double fConst0;
	double fConst1;
	double fRec1[2];
	int iVec0[2];
	double fConst2;
	int iRec2[2];
	double fRec0[2];

public:
	NoiseGate();
	virtual ~NoiseGate() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
