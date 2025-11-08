#include <math.h>
#include <algorithm>
#include "Fuzz.h"

Fuzz::Fuzz()
{
	Name = "Fuzz";
	Description = "Simple fuzz simulation";

	NumParameters = FUZZ_NUMPARAMETERS;
	CreateParameters(NumParameters);

	level = 0.5;
	fuzz = 0.5;
	octave = 0;
	asymmetry = 0.5;

	Parameters[FUZZ_FUZZ].Name = "Fuzz";
	Parameters[FUZZ_FUZZ].SourceVariable = &fuzz;
	Parameters[FUZZ_FUZZ].DefaultValue = fuzz;
	Parameters[FUZZ_FUZZ].Description = "Fuzz effect strength";

	Parameters[FUZZ_LEVEL].Name = "Level";
	Parameters[FUZZ_LEVEL].SourceVariable = &level;
	Parameters[FUZZ_LEVEL].DefaultValue = level;
	Parameters[FUZZ_LEVEL].Description = "Effect output level";

	Parameters[FUZZ_OCTAVE].Name = "Octave";
	Parameters[FUZZ_OCTAVE].ParameterType = PARAMETER_TYPE_BOOL;
	Parameters[FUZZ_OCTAVE].SourceVariable = &octave;
	Parameters[FUZZ_OCTAVE].DefaultValue = octave;
	Parameters[FUZZ_OCTAVE].Description = "Enable/disable octave effect";

	Parameters[FUZZ_ASYMMETRY].Name = "Bias";
	Parameters[FUZZ_ASYMMETRY].SourceVariable = &asymmetry;
	Parameters[FUZZ_ASYMMETRY].DefaultValue = asymmetry;
	Parameters[FUZZ_ASYMMETRY].IsAdvanced = true;
	Parameters[FUZZ_ASYMMETRY].Description = "DC offset bias";
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