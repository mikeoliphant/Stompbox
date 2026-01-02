#include <math.h>
#include <algorithm>
#include "Fuzz.h"

Fuzz::Fuzz()
{
	Name = "Fuzz";
	Description = "Simple fuzz simulation";

	level = 0.5;
	fuzz = 0.5;
	octave = 0;
	asymmetry = 0.5;

	auto& fuzzParam = AddParameter();
	fuzzParam.Name = "Fuzz";
	fuzzParam.SourceVariable = &fuzz;
	fuzzParam.DefaultValue = fuzz;
	fuzzParam.Description = "Fuzz effect strength";

	auto& levelParam = AddParameter();
	levelParam.Name = "Level";
	levelParam.SourceVariable = &level;
	levelParam.DefaultValue = level;
	levelParam.Description = "Effect output level";

	auto& octaveParam = AddParameter();
	octaveParam.Name = "Octave";
	octaveParam.ParameterType = PARAMETER_TYPE_BOOL;
	octaveParam.SourceVariable = &octave;
	octaveParam.DefaultValue = octave;
	octaveParam.Description = "Enable/disable octave effect";

	auto& asymParam = AddParameter();
	asymParam.Name = "Bias";
	asymParam.SourceVariable = &asymmetry;
	asymParam.DefaultValue = asymmetry;
	asymParam.IsAdvanced = true;
	asymParam.Description = "DC offset bias";
}

void Fuzz::init(int samplingFreq)
{
	StompBox::init(samplingFreq);
}

void Fuzz::compute(int count, float* input0, float* output0)
{

	//double shape = 1 - (fuzz * 0.99);

	//double scale = (1 / atan(1 / shape));

	double mult = 5 + (fuzz * 100);
	double scale = level * 0.25;
	double offset = asymmetry * 0.8;

	for (int i = 0; i < count; i++)
	{
		env = std::max(env, abs(input0[i]));

		output0[i] = (float)(atan((input0[i] + (env * offset))  * mult) * scale);

		//output0[i] = (scale * atan((input0[i] / clip) / shape)) * clip;

		//output0[i] = (output0[i] * 0.1) + (fb * 0.9f);

		//fb = output0[i];

		if (octave != 0)
		{
			if (output0[i] < 0)
				output0[i] = -output0[i];
		}

		env *= .99f;
	}
}