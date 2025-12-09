#pragma once

#include "StompBox.h"

enum
{
	COMPRESSOR_THRESHOLD,
	COMPRESSOR_RATIO,
	COMPRESSOR_ATTACK,
	COMPRESSOR_RELEASE,
	COMPRESSOR_WETDRY,
	COMPRESSOR_COMPRESSION,
	COMPRESSOR_NUMPARAMETERS
};

class Compressor : public StompBox
{
private:
	int fSamplingFreq;
	float compression;

	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fVslider0;
	float fConst0;
	float fConst1;
	float fConst2;
	float fRec1[2];
	float fConst3;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fHslider2;
	float fRec0[2];
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fHslider4;
	FAUSTFLOAT fVbargraph0;

public:
	Compressor();
	virtual ~Compressor() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, float* input, float* output);
};
