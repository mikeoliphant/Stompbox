#include <math.h>
#include <algorithm>
#include "Tremolo.h"

Tremolo::Tremolo()
{
	Name = "Tremolo";
	Description = "Tremolo (volume modulation) effect";

	NumParameters = TREMOLO_NUMPARAMETERS;
	CreateParameters(NumParameters);

	speed = 5;
	Parameters[TREMOLO_SPEED].Name = "Speed";
	Parameters[TREMOLO_SPEED].SourceVariable = &speed;
	Parameters[TREMOLO_SPEED].MinValue = 1;
	Parameters[TREMOLO_SPEED].MaxValue = 20;
	Parameters[TREMOLO_SPEED].Step = 0.1f;
	Parameters[TREMOLO_SPEED].DefaultValue = speed;
	Parameters[TREMOLO_SPEED].DisplayFormat = "{0:0.0}hz";
	Parameters[TREMOLO_SPEED].Description = "Modulation speed";

	depth = 0.8f;
	Parameters[TREMOLO_DEPTH].Name = "Depth";
	//Parameters[TREMOLO_DEPTH].MinValue = -12;
	//Parameters[TREMOLO_DEPTH].MaxValue = 0;
	Parameters[TREMOLO_DEPTH].DefaultValue = depth;
	Parameters[TREMOLO_DEPTH].SourceVariable = &depth;
	Parameters[TREMOLO_DEPTH].Description = "Amount of modulation volume variance";

	shape = 0.5;
	Parameters[TREMOLO_SHAPE].Name = "Shape";
	Parameters[TREMOLO_SHAPE].MinValue = .05f;
	Parameters[TREMOLO_SHAPE].DefaultValue = shape;
	Parameters[TREMOLO_SHAPE].SourceVariable = &shape;
	Parameters[TREMOLO_SHAPE].Description = "Smooth vs squared modulation shape";
}

void Tremolo::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	time = 0;
	tfrac = 1.0f / (float)samplingFreq;
}

void Tremolo::compute(int count, float* input0, float* output0)
{
	float linearGain = (float)pow(10.0, (0.05 * double(shape)));
	//double linearDepth = pow(10.0, (0.05 * double(depth)));

	for (int i = 0; i < count; i++)
	{
		//output0[i] = input0[i] * (1 - (linearDepth * 2) + (sin(speed * time) * linearDepth)) * linearGain;   // ** Wrong, but interesting

		//double lfo = sin(speed * time);
		//lfo = (.5 * lfo) + (.5 * (lfo > 0 ? 1 : -1));

		float lfo = (float)((1 / atan(1 / shape))* atan(sin(time * 2 * M_PI * speed) / shape));

		output0[i] = input0[i] * (1 - ((depth + (lfo * depth))) / 2) * linearGain;

		time += tfrac;
	}
}