#include "StompBox.h"
#include <algorithm>

float StompBoxParameter::GetValue() const
{
	return Stomp->GetParameterValue(*this);
}

void StompBoxParameter::SetValue(float value)
{
	return Stomp->SetParameterValue(*this, value);
}

StompBox::StompBox()
{
}

StompBox::~StompBox()
{
}

StompBoxParameter* StompBox::GetParameter(const std::string& name)
{
	for (auto& param : Parameters)
	{
		if (param.Name == name)
			return &param;
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

void StompBox::SetParameterValue(StompBoxParameter &param, float value)
{
	*(param.SourceVariable) = std::clamp(value, param.MinValue, param.MaxValue);
}

float StompBox::GetParameterValue(const StompBoxParameter& param)
{
	return *(param.SourceVariable);
}

void StompBox::SetBPM(float newBpm)
{
	bpm = newBpm;

	UpdateBPM();
}

void StompBox::UpdateBPM()
{
	for (auto& param : Parameters)
	{
		if ((param.BPMSyncNumerator != 0) && (param.BPMSyncDenominator != 0))
		{
			(*param.SourceVariable) = ((60.0f / bpm) * ((float)param.BPMSyncNumerator / (float)param.BPMSyncDenominator)) * 1000;
		}
	}
}
