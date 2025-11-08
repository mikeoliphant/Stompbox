#pragma once

#include "StompBox.h"

enum
{
	FUZZ_FUZZ,
	FUZZ_OCTAVE,
	FUZZ_LEVEL,
	FUZZ_ASYMMETRY,
	FUZZ_NUMPARAMETERS
};

class Fuzz : public StompBox
{
private:
	float level;
	float fuzz;
	float octave;
	float asymmetry;

	float fb;
	float env;
public:
	Fuzz();
	~Fuzz() {}

	virtual void init(int samplingFreq);
	virtual void compute(int count, float* input, float* output);
};
