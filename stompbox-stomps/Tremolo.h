#pragma once

#include "StompBox.h"

enum
{
	TREMOLO_SPEED,
	TREMOLO_DEPTH,
	TREMOLO_SHAPE,
	TREMOLO_NUMPARAMETERS
};

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
