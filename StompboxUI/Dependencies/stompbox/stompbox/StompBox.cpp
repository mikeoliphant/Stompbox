#include "StompBox.h"
#include <algorithm>

StompBox::StompBox()
{
}

StompBox::~StompBox()
{
	if (Parameters != nullptr)
	{
		delete[] Parameters;
	}
}

void StompBox::CreateParameters(size_t numParameters)
{
	Parameters = new StompBoxParameter[numParameters];

	for (size_t i = 0; i < NumParameters; i++)
	{
		Parameters[i].Stomp = this;
	}
}

StompBoxParameter* StompBox::GetParameter(size_t id)
{
	return &Parameters[id];
}

StompBoxParameter* StompBox::GetParameter(const std::string& name)
{
	for (size_t i = 0; i < NumParameters; i++)
	{
		if (Parameters[i].Name == name)
			return &Parameters[i];
	}

	StompBoxParameter* parameter = nullptr;

	if (InputGain != nullptr)
		parameter = InputGain->GetParameter(name);

	if ((parameter == nullptr) && (OutputVolume != nullptr))
		parameter = OutputVolume->GetParameter(name);

	return parameter;
}

void StompBox::init(int newSamplingFreq)
{
	samplingFreq = (float)newSamplingFreq;
}

void StompBox::SetParameterValue(size_t id, float value)
{
	SetParameterValue(&(Parameters[id]), value);
}

void StompBox::SetParameterValue(StompBoxParameter *param, float value)
{
	*(param->SourceVariable) = std::clamp(value, param->MinValue, param->MaxValue);
}

float StompBox::GetParameterValue(size_t id)
{
	return GetParameterValue(&(Parameters[id]));
}

float StompBox::GetParameterValue(StompBoxParameter* param)
{
	return *(param->SourceVariable);
}

void StompBox::SetBPM(float newBpm)
{
	bpm = newBpm;

	UpdateBPM();
}

void StompBox::UpdateBPM()
{
	for (size_t i = 0; i < NumParameters; i++)
	{
		if ((Parameters[i].BPMSyncNumerator != 0) && (Parameters[i].BPMSyncDenominator != 0))
		{
			(*Parameters[i].SourceVariable) = ((60.0f / bpm) * ((float)Parameters[i].BPMSyncNumerator / (float)Parameters[i].BPMSyncDenominator)) * 1000;
		}
	}
}
