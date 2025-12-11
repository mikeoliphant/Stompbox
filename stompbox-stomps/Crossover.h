#pragma once

#include "StompBox.h"

class Crossover
{
private:
	int fSampleRate;
	double crossoverFreq;
	double fConst0;
	double fRec3[2];
	double fRec4[2];
	double fRec0[2];
	double fRec1[2];
	double fRec7[2];
	double fRec8[2];

public:
	Crossover();
	~Crossover() {}
	double GetCrossoverFrequency()
	{
		return crossoverFreq;
	};
	void SetCrossoverFrequency(double freq)
	{
		crossoverFreq = freq;
	};
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output0, FAUSTFLOAT* output1);
};