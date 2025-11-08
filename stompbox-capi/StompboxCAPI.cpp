#include "string.h"

#include "PluginProcessor.h"
#include "StompBox.h"
#include "StompboxCAPI.h"

// String vector helpers

size_t GetStringVectorSize(void* strVec)
{
    return (*(std::vector<std::string> *)strVec).size();
}

const char* GetStringVectorValue(void* strVec, size_t index)
{
    return (*(std::vector<std::string> *)strVec)[index].c_str();
}


// Processor methods

void* CreateProcessor(const wchar_t* dataPath, bool dawMode)
{
    return (void *)new PluginProcessor(dataPath, dawMode);
}

void DeleteProcessor(void* processor)
{
    delete (PluginProcessor*)processor;
}

void InitProcessor(void* processor, float sampleRate)
{
    ((PluginProcessor*)processor)->Init(sampleRate);
}

void SetMaxAudioBufferSize(void* processor, size_t numSamples)
{
    ((PluginProcessor*)processor)->SetMaxAudioBufferSize(numSamples);
}

void StartServer(void* processor)
{
    ((PluginProcessor*)processor)->StartServer();
}

void Process(void* processor, float* input, float* output, size_t bufferSize)
{
    ((PluginProcessor*)processor)->Process(input, output, bufferSize);
}

bool IsPresetLoading(void* processor)
{
    return ((PluginProcessor*)processor)->IsPresetLoading();
}

void HandleCommand(void* processor, const char* cmd)
{
    ((PluginProcessor*)processor)->HandleCommand(cmd);
}

bool HandleMidiCommand(void* processor, int midiCommand, int midiData1, int midiData2)
{
    return ((PluginProcessor*)processor)->HandleMidiCommand(midiCommand, midiData1, midiData2);
}


void SetBPM(void* processor, float bpm)
{
    ((PluginProcessor*)processor)->SetBPM(bpm);
}

const PATH_STR GetDataPath(void* processor)
{
    return ((PluginProcessor*)processor)->GetDataPath().c_str();
}

void* GetAllPlugins(void* processor)
{
    return (void *)&(((PluginProcessor*)processor)->GetPluginFactory()->GetAllPlugins());
}

const char* GetGlobalChain(void* processor)
{
    auto chain = ((PluginProcessor*)processor)->GetGlobalChain();

    size_t len = chain.length() + 1;
    char* buff = (char*)CoTaskMemAlloc(len);
    strcpy(buff, chain.c_str());

    return buff;
}

void* CreatePlugin(void* processor, const char* id)
{
    StompBox* stomp = ((PluginProcessor*)processor)->CreatePlugin(id);

    if (stomp == nullptr)
        return nullptr;

    return (void *)stomp;
}

const char* GetPluginSlot(void* processor, const char* slotName)
{
    StompBox* stomp = ((PluginProcessor*)processor)->GetPluginSlot(slotName);

    if (stomp == nullptr)
        return nullptr;

    return stomp->ID.c_str();
}

void SetPluginSlot(void* processor, const char* slotName, const char* pluginID)
{
    ((PluginProcessor*)processor)->SetPluginSlot(slotName, pluginID);
}

size_t GetPluginVectorSize(void* plugVec)
{
    return (*(std::vector<StompBox*> *)plugVec).size();
}

void* GetPluginVectorValue(void* plugVec, size_t index)
{
    return (*(std::vector<StompBox*> *)plugVec)[index];
}


void* GetChainPlugins(void* processor, const char* chainName)
{
    return (void*)&(((PluginProcessor*)processor)->GetChain(chainName)->Plugins.GetRead());
}

void* GetPresets(void* processor)
{
    return (void*)&(((PluginProcessor*)processor)->GetPresets());
}

const char* GetCurrentPreset(void* processor)
{
    return ((PluginProcessor*)processor)->GetCurrentPreset().c_str();
}

void LoadPreset(void* processor, const char* presetName)
{
    ((PluginProcessor*)processor)->LoadPreset(presetName);
}

const char* DumpSettings(void* processor)
{
    auto settings = ((PluginProcessor*)processor)->DumpSettings();

    size_t len = settings.length() + 1;
    char* buff = (char*)CoTaskMemAlloc(len);
    strcpy(buff, settings.c_str());

    return buff;
}

const char* DumpProgram(void* processor)
{
    auto program = ((PluginProcessor*)processor)->DumpProgram();

    size_t len = program.length() + 1;
    char* buff = (char*)CoTaskMemAlloc(len);
    strcpy(buff, program.c_str());

    return buff;
}


// Plugin methods

const char* GetPluginName(void* plugin)
{
    return ((StompBox *)plugin)->Name.c_str();
}

const char* GetPluginID(void* plugin)
{
    return ((StompBox*)plugin)->ID.c_str();
}

bool GetPluginEnabled(void* plugin)
{
    return ((StompBox*)plugin)->Enabled;
}

void SetPluginEnabled(void* plugin, bool enabled)
{
    ((StompBox*)plugin)->Enabled = enabled;
}

const char* GetPluginDescription(void* plugin)
{
    return ((StompBox*)plugin)->Description.c_str();
}

const char* GetPluginBackgroundColor(void* plugin)
{
    return ((StompBox*)plugin)->BackgroundColor.c_str();
}

const char* GetPluginForegroundColor(void* plugin)
{
    return ((StompBox*)plugin)->ForegroundColor.c_str();
}

bool GetPluginIsUserSelectable(void* plugin)
{
    return ((StompBox*)plugin)->IsUserSelectable;
}

size_t GetPluginNumParameters(void* plugin)
{
    StompBox* stomp = (StompBox*)plugin;

    size_t numParams = 0;

    if (stomp->InputGain != nullptr)
    {
        numParams++;
    }

    numParams += stomp->NumParameters;

    if (stomp->OutputVolume != nullptr)
    {
        numParams++;
    }

    return numParams;
}

void* GetPluginParameter(void* plugin, size_t index)
{
    StompBox* stomp = (StompBox*)plugin;

    if (stomp->InputGain != nullptr)
    {
        if (index == 0)
            return (void*)(&stomp->InputGain->Parameters[0]);

        index--;
    }

    if (index < stomp->NumParameters)
        return (void*)(&stomp->Parameters[index]);

    index -= stomp->NumParameters;

    if (index > 0)
        return nullptr;

    return (void*)(&stomp->OutputVolume->Parameters[0]);
}

// Parameter methods

float GetParameterValue(void *parameter)
{
    return ((StompBoxParameter*)(void*)parameter)->GetValue();
}

void SetParameterValue(void* parameter, float value)
{
    return ((StompBoxParameter*)(void*)parameter)->SetValue(value);
}

const char* GetParameterName(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->Name.c_str();
}

const char* GetParameterDescription(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->Description.c_str();
}

float GetParameterMinValue(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->MinValue;
}

float GetParameterMaxValue(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->MaxValue;
}

float GetParameterDefaultValue(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->DefaultValue;
}

float GetParameterRangePower(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->RangePower;
}

int GetParameterType(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->ParameterType;
}

void* GetParameterEnumValues(void* parameter)
{
    return (void *)(((StompBoxParameter*)(void*)parameter)->EnumValues);
}


bool GetParameterCanSyncToHostBPM(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->CanSyncToHostBPM;
}

int GetParameterBPMSyncNumerator(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->BPMSyncNumerator;
}

int GetParameterBPMSyncDenominator(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->BPMSyncDenominator;
}

void SetParameterBPMSyncNumerator(void* parameter, int numerator)
{
	((StompBoxParameter*)(void*)parameter)->BPMSyncNumerator = numerator;

    ((StompBoxParameter*)(void*)parameter)->Stomp->UpdateBPM();
}

void SetParameterBPMSyncDenominator(void* parameter, int denom)
{
	((StompBoxParameter*)(void*)parameter)->BPMSyncDenominator = denom;
    ((StompBoxParameter*)(void*)parameter)->Stomp->UpdateBPM();
}

bool GetParameterIsAdvanced(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->IsAdvanced;
}

bool GetParameterIsOutput(void* parameter)
{
    return ((StompBoxParameter*)(void*)parameter)->IsOutput;
}

const char* GetParameterFilePath(void* parameter)
{
	return ((StompBoxParameter*)(void*)parameter)->FilePath.c_str();
}

const char* GetParameterDisplayFormat(void* parameter)
{
    return ((StompBoxParameter*)(void*)parameter)->DisplayFormat;
}

