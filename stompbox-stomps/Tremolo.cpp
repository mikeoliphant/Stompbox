#include <math.h>
#include <algorithm>
#include "Tremolo.h"

Tremolo::Tremolo()
{
	Name = "Tremolo";
	Description = "Tremolo (volume modulation) effect";

	speed = 5;
	depth = 0.8f;
	shape = 0.5;

	auto& speedParam = AddParameter();
	speedParam.Name = "Speed";
	speedParam.SourceVariable = &speed;
	speedParam.MinValue = 1;
	speedParam.MaxValue = 20;
	speedParam.Step = 0.1f;
	speedParam.DefaultValue = speed;
	speedParam.DisplayFormat = "{0:0.0}hz";
	speedParam.Description = "Modulation speed";

	auto& depthParam = AddParameter();
	depthParam.Name = "Depth";
	//Parameters[TREMOLO_DEPTH].MinValue = -12;
	//Parameters[TREMOLO_DEPTH].MaxValue = 0;
	depthParam.DefaultValue = depth;
	depthParam.SourceVariable = &depth;
	depthParam.Description = "Amount of modulation volume variance";

	auto& shapeParam = AddParameter();
	shapeParam.Name = "Shape";
	shapeParam.MinValue = .05f;
	shapeParam.DefaultValue = shape;
	shapeParam.SourceVariable = &shape;
	shapeParam.Description = "Smooth vs squared modulation shape";
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