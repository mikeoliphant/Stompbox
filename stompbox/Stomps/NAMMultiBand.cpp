#include "NAMMultiBand.h"
#include "Gain.h"

NAMMultiBand::NAMMultiBand(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
	nam(folderName, fileExtensions, basePath)
{
    Name = "NAMMulti";

	nam.InputGain = new Gain(0, -40, 40);
	nam.OutputVolume = new Gain(0, -40, 40);
	nam.OutputVolume->Parameters[GAIN_GAIN].Name = "Volume";

    NumParameters = NAMMULTIBAND_NUMPARAMETERS;
    CreateParameters(NumParameters);

	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].Name = "Freq";
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].MinValue = 10;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].MaxValue = 1000;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].SourceVariable = &crossoverFreq;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].DefaultValue = crossoverFreq;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].RangePower = 3;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].DisplayFormat = "{0:0}hz";

	memcpy(&Parameters[NAMMULTIBAND_MODEL_GAIN], &nam.InputGain->Parameters[GAIN_GAIN], sizeof(StompBoxParameter));
	memcpy(&Parameters[NAMMULTIBAND_MODEL_VOLUME], &nam.OutputVolume->Parameters[GAIN_GAIN], sizeof(StompBoxParameter));

	memcpy(&Parameters[NAMMULTIBAND_MODEL], &nam.Parameters[NAM_MODEL], sizeof(StompBoxParameter));
	Parameters[NAMMULTIBAND_MODEL].Stomp = this;
}

void NAMMultiBand::init(int samplingFreq)
{
    nam.init(samplingFreq);
	nam.InputGain->init(samplingFreq);
	nam.OutputVolume->init(samplingFreq);

	crossover.init(samplingFreq);
}

float NAMMultiBand::GetParameterValue(StompBoxParameter* parameter)
{
	if (parameter == &Parameters[NAMMULTIBAND_MODEL])
	{
		return nam.GetParameter(NAM_MODEL)->GetValue();
	}

	return *(parameter->SourceVariable);
}


void NAMMultiBand::SetParameterValue(StompBoxParameter *parameter, float value)
{
	StompBox::SetParameterValue(parameter, value);

	if (parameter == &Parameters[NAMMULTIBAND_MODEL])
	{
		nam.GetParameter(NAM_MODEL)->SetValue(value);
	}
}

void NAMMultiBand::compute(int count, float* input, float* output)
{
	crossover.SetCrossoverFrequency(crossoverFreq);
	crossover.compute(count, input, splitBuf[0], splitBuf[1]);

	nam.InputGain->compute(count, splitBuf[1], splitBuf[1]);

	// Run nam on high frequencies
	nam.compute(count, splitBuf[1], output);

	nam.OutputVolume->compute(count, output, output);

	// Blend in passthrough of low frequencies
	for (int i = 0; i < count; i++)
	{
		output[i] += splitBuf[0][i];
	}
}
