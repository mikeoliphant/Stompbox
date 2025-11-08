#pragma once

#include "StompBox.h"

enum
{
	DELAY_DELAY,
	DELAY_LEVEL,
	DELAY_FEEDBACK,
	DELAY_WARMTH,
	DELAY_LOWCUT,
	DELAY_NUMPARAMETERS
};

class Delay : public StompBox
{
private:
	int fSamplingFreq;

	float delay;
	float level;
	float feedback;
	double hipass;
	double lowpass;
	float warmth;
	float hilo;
	float wet;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fRec3[2];
	double fRec4[2];
	double fRec5[2];
	double fRec6[2];
	int IOTA;
	double fVec0[262144];
	double fRec2[3];
	double fRec1[3];
	double fRec0[2];

public:
	Delay();
	virtual ~Delay() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
