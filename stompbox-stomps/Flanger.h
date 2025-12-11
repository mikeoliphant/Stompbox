#pragma once

#include "StompBox.h"

enum
{
	FLANGER_RATE,
	FLANGER_DEPTH,
	FLANGER_FEEDBACK,
	FLANGER_NUMPARAMETERS
};

class Flanger : public StompBox
{
private:
	int fSamplingFreq;

	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	int iVec0[2];
	int IOTA;
	double fVec1[4096];
	double fConst0;
	double fConst1;
	FAUSTFLOAT fHslider2;
	double fRec1[2];
	double fRec2[2];
	double fRec0[2];

public:
	Flanger();
	virtual ~Flanger() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
