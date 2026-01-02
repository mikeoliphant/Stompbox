#pragma once

#include "StompBox.h"

class Tremolo : public StompBox
{
private:
	float speed;
	float depth;
	float shape;
	float time;
	float tfrac;
public:
	Tremolo();
	~Tremolo() {}

	virtual void init(int samplingFreq);
	virtual void compute(int count, float* input, float* output);
};
