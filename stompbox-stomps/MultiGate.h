#pragma once

#include "StompBox.h"

enum
{
	MULTIGATE_THRESHOLD,
	MULTIGATE_STRENGTH,
	MULTIGATE_ATTACK,
	MULTIGATE_RELEASE,
	MULTIGATE_SOFTNESS,
	MULTIGATE_NUMPARAMETERS
};

class MultiGate : public StompBox
{
private:
	int fSampleRate;

	float threshold;
	float attackMS;
	float releaseMS;
	float softness;
	float strength;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fRec8[2];
	double fConst4;
	double fRec9[2];
	double fConst5;
	double fRec4[2];
	double fRec5[2];
	double fConst6;
	double fConst7;
	double fConst8;
	double fRec1[2];
	double fConst9;
	double fRec2[2];
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fRec12[2];
	double fConst14;
	double fRec13[2];
	double fConst15;
	double fRec16[2];
	double fRec17[2];
	double fConst16;
	double fRec0[2];
	double fRec21[2];
	double fRec22[2];
	double fRec20[2];
	double fRec28[2];
	double fRec29[2];
	double fRec25[2];
	double fRec26[2];
	double fRec31[2];
	double fRec32[2];
	double fRec35[2];
	double fRec36[2];
	double fRec24[2];
	double fRec40[2];
	double fRec41[2];
	double fRec39[2];

public:
	MultiGate();
	virtual ~MultiGate() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
