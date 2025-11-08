#pragma once

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include <iostream>
#include <string>
#include <vector>
#include <functional>

#ifdef __linux__
#define __forceinline __attribute__((always_inline))
#endif

enum
{
	PARAMETER_TYPE_KNOB,
	PARAMETER_TYPE_BOOL,
	PARAMETER_TYPE_INT,
	PARAMETER_TYPE_VSLIDER,
	PARAMETER_TYPE_ENUM,
	PARAMETER_TYPE_FILE,
	PARAMETER_TYPE_POWER
};

struct StompBoxParameter;

class StompBox
{
protected:
	float samplingFreq = 0;
	float bpm;
	int bufferSize;

public:
	bool Enabled = true;
	StompBoxParameter* Parameters = nullptr;
	StompBox* InputGain = nullptr;
	StompBox* OutputVolume = nullptr;
	size_t NumParameters;
	std::string Name;
	std::string ID;
	std::string Description;
	std::string BackgroundColor = "#eeeeee";
	std::string ForegroundColor = "#000000";
	bool IsUserSelectable = true;
	bool NeedsInit = true;
	bool EnabledIsDirty = false;
	bool ParamIsDirty = false;
	std::function<void(int, int, int)> MidiCallback = nullptr;

	StompBox();
	virtual ~StompBox();
	virtual void CreateParameters(size_t numParameters);
	virtual StompBoxParameter* GetParameter(size_t id);
	virtual StompBoxParameter* GetParameter(const std::string& name);
	virtual void SetParameterValue(size_t id, float value) final;
	virtual void SetParameterValue(StompBoxParameter *param, float value);
	virtual float GetParameterValue(size_t id);
	virtual float GetParameterValue(StompBoxParameter* param);
	virtual void HandleCommand(const std::vector<std::string>& commandWords)
	{
		(void)commandWords;
	}
	virtual void SetBPM(float bpm);
	virtual void UpdateBPM();
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int newSamplingFreq)
	{
		(void)newSamplingFreq;
	}
	virtual void instanceClear() {};
	virtual void compute(int count, float* input, float* output)
	{
		(void)count;
		(void)input;
		(void)output;
	}
};

struct StompBoxParameter
{
	std::string Name = "Unnamed";
	std::string Description;
	StompBox* Stomp = nullptr;
	float* SourceVariable = nullptr;
	float MinValue = 0;
	float MaxValue = 1;
	float DefaultValue = 0.5;
	float RangePower = 1;
	const char* DisplayFormat = "{0:0.00}";
	float Step = 0.01f;
	int ParameterType = PARAMETER_TYPE_KNOB;
	bool CanSyncToHostBPM = false;
	int BPMSyncNumerator = 0;
	int BPMSyncDenominator = 0;
	bool IsAdvanced = false;
	bool IsOutput = false;
	std::string FilePath;
	const std::vector<std::string>* EnumValues = nullptr;
	bool IsDirty = false;
	bool SuppressSave = false;
	float GetValue()
	{
		return Stomp->GetParameterValue(this);
	}
	void SetValue(float value)
	{
		return Stomp->SetParameterValue(this, value);
	}
};
