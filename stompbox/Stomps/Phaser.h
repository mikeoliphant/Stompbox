#pragma once

#include "StompBox.h"

enum
{
	PHASER_SPEED,
	PHASER_DEPTH,
	PHASER_FEEDBACK,
	PHASER_FREQWIDTH,
	PHASER_RATIO,
	PHASER_NUMPARAMETERS
};

class Phaser : public StompBox
{
private:
	int fSamplingFreq;

	float freq;
	float depth;
	float fb;
	float width;
	float frqwidth;
	double frqmin;
	double frqmax;
	float fratio;

	int iVec0[2];
	double fConst0;
	double fConst1;
	double fConst2;
	double fRec3[2];
	double fRec4[2];
	double fRec2[3];
	double fRec1[3];
	double fRec0[2];
public:
	Phaser();
	virtual ~Phaser() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
