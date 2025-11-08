#pragma once

#include "StompBox.h"

enum
{
	REVERB_ROOMSIZE,
	REVERB_DECAY,
	REVERB_WETDRY,
	REVERB_NUMPARAMETERS
};


class FreeVerb : public StompBox
{
private:
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	FAUSTFLOAT fVslider2;
	double fRec9[2];
	int IOTA;
	double fVec0[8192];
	int fSampleRate;
	double fConst0;
	int iConst1;
	double fRec8[2];
	double fRec11[2];
	double fVec1[8192];
	int iConst2;
	double fRec10[2];
	double fRec13[2];
	double fVec2[8192];
	int iConst3;
	double fRec12[2];
	double fRec15[2];
	double fVec3[8192];
	int iConst4;
	double fRec14[2];
	double fRec17[2];
	double fVec4[8192];
	int iConst5;
	double fRec16[2];
	double fRec19[2];
	double fVec5[8192];
	int iConst6;
	double fRec18[2];
	double fRec21[2];
	double fVec6[8192];
	int iConst7;
	double fRec20[2];
	double fRec23[2];
	double fVec7[8192];
	int iConst8;
	double fRec22[2];
	double fVec8[4096];
	int iConst9;
	double fRec6[2];
	double fVec9[2048];
	int iConst10;
	double fRec4[2];
	double fVec10[2048];
	int iConst11;
	double fRec2[2];
	double fVec11[1024];
	int iConst12;
	double fRec0[2];

public:
	FreeVerb();
	virtual ~FreeVerb() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
