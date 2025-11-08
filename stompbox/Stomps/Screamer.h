#pragma once

#include "StompBox.h"

enum
{
	SCREAMER_DRIVE,
	SCREAMER_TONE,
	SCREAMER_LEVEL,
	SCREAMER_NUMPARAMETERS
};

class TS9 : public StompBox
{
private:
	int fSamplingFreq;

	FAUSTFLOAT fVslider0;
	double fRec0[2];
	double fConst0;
	double fConst1;
	FAUSTFLOAT fVslider1;
	double fVec0[2];
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	FAUSTFLOAT fVslider2;
	double fRec2[2];
	double fVec1[2];
	double fRec1[2];
public:
	TS9();
	virtual ~TS9() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};