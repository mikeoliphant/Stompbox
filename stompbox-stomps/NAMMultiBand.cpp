#include "NAMMultiBand.h"
#include "Gain.h"

NAMMultiBand::NAMMultiBand(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
	nam(folderName, fileExtensions, basePath)
{
    Name = "NAMMulti";

	nam.InputGain = new Gain(0, -40, 40);
	nam.OutputVolume = new Gain(0, -40, 40);
	nam.OutputVolume->GetParameter("Gain")->Name = "Volume";

	auto& crossoverParam = AddParameter();
	crossoverParam.Name = "Freq";
	crossoverParam.MinValue = 10;
	crossoverParam.MaxValue = 1000;
	crossoverParam.SourceVariable = &crossoverFreq;
	crossoverParam.DefaultValue = crossoverFreq;
	crossoverParam.RangePower = 3;
	crossoverParam.DisplayFormat = "{0:0}hz";

	AddParameter(*nam.GetParameter("Gain"));
	AddParameter(*nam.GetParameter("Volume"));

	auto& modelParam = AddParameter(*nam.GetParameter("Model"));
	modelParam.Stomp = this;
}

void NAMMultiBand::init(int samplingFreq)
{
    nam.init(samplingFreq);
	nam.InputGain->init(samplingFreq);
	nam.OutputVolume->init(samplingFreq);

	crossover.init(samplingFreq);
}

float NAMMultiBand::GetParameterValue(const StompBoxParameter& parameter)
{
	if (parameter.Name == "Model")
	{
		return nam.GetParameter("Model")->GetValue();
	}

	return *(parameter.SourceVariable);
}


void NAMMultiBand::SetParameterValue(StompBoxParameter &parameter, float value)
{
	StompBox::SetParameterValue(parameter, value);

	if (parameter.Name == "Model")
	{
		nam.GetParameter("Model")->SetValue(value);
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
