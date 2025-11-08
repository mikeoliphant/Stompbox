#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _MSC_VER
#define STOMPBOX_EXTERN extern __declspec(dllexport)
#define PATH_STR wchar_t*
#include "Combaseapi.h"
#else
#define STOMPBOX_EXTERN extern
#define PATH_STR char*
#define CoTaskMemAlloc(p) malloc(p)
#endif

STOMPBOX_EXTERN size_t GetStringVectorSize(void* strVec);

STOMPBOX_EXTERN const char* GetStringVectorValue(void* strVec, size_t index);

STOMPBOX_EXTERN void* CreateProcessor(const wchar_t* dataPath, bool dawMode);

STOMPBOX_EXTERN void DeleteProcessor(void* processor);

STOMPBOX_EXTERN void InitProcessor(void* processor, float sampleRate);

STOMPBOX_EXTERN void SetMaxAudioBufferSize(void* processor, size_t numSamples);

STOMPBOX_EXTERN void StartServer(void* processor);

STOMPBOX_EXTERN void Process(void* processor, float* input, float* output, size_t bufferSize);

STOMPBOX_EXTERN bool IsPresetLoading(void* processor);

STOMPBOX_EXTERN void HandleCommand(void* processor, const char* cmd);

STOMPBOX_EXTERN bool HandleMidiCommand(void* processor, int midiCommand, int midiData1, int midiData2);

STOMPBOX_EXTERN void SetBPM(void* processor, float bpm);

STOMPBOX_EXTERN const PATH_STR GetDataPath(void* processor);

STOMPBOX_EXTERN void *GetAllPlugins(void* processor);

STOMPBOX_EXTERN const char* GetGlobalChain(void* processor);

STOMPBOX_EXTERN const char* GetPluginSlot(void* processor, const char* slotName);

STOMPBOX_EXTERN void SetPluginSlot(void* processor, const char* slotName, const char* pluginID);

STOMPBOX_EXTERN size_t GetPluginVectorSize(void* plugVec);

STOMPBOX_EXTERN void* GetPluginVectorValue(void* plugVec, size_t index);

STOMPBOX_EXTERN void* GetChainPlugins(void* processor, const char* chainName);

STOMPBOX_EXTERN void* CreatePlugin(void* processor, const char* id);

STOMPBOX_EXTERN void* GetPresets(void* processor);

STOMPBOX_EXTERN const char* GetCurrentPreset(void* processor);

STOMPBOX_EXTERN void LoadPreset(void* processor, const char* presetName);

STOMPBOX_EXTERN const char* DumpSettings(void* processor);

STOMPBOX_EXTERN const char* DumpProgram(void* processor);

STOMPBOX_EXTERN const char* GetPluginName(void* plugin);

STOMPBOX_EXTERN const char* GetPluginID(void* plugin);

STOMPBOX_EXTERN bool GetPluginEnabled(void* plugin);

STOMPBOX_EXTERN void SetPluginEnabled(void* plugin, bool enabled);

STOMPBOX_EXTERN const char* GetPluginDescription(void* plugin);

STOMPBOX_EXTERN const char* GetPluginBackgroundColor(void* plugin);

STOMPBOX_EXTERN const char* GetPluginForegroundColor(void* plugin);

STOMPBOX_EXTERN bool GetPluginIsUserSelectable(void* plugin);

STOMPBOX_EXTERN size_t GetPluginNumParameters(void* plugin);

STOMPBOX_EXTERN void* GetPluginParameter(void* plugin, size_t index);

STOMPBOX_EXTERN float GetParameterValue(void* parameter);

STOMPBOX_EXTERN void SetParameterValue(void* parameter, float value);

STOMPBOX_EXTERN const char* GetParameterName(void* parameter);

STOMPBOX_EXTERN const char* GetParameterDescription(void* parameter);

STOMPBOX_EXTERN float GetParameterMinValue(void* parameter);

STOMPBOX_EXTERN float GetParameterMaxValue(void* parameter);

STOMPBOX_EXTERN float GetParameterDefaultValue(void* parameter);

STOMPBOX_EXTERN float GetParameterRangePower(void* parameter);

STOMPBOX_EXTERN int GetParameterType(void* parameter);

STOMPBOX_EXTERN void* GetParameterEnumValues(void* parameter);

STOMPBOX_EXTERN bool GetParameterCanSyncToHostBPM(void* parameter);

STOMPBOX_EXTERN int GetParameterBPMSyncNumerator(void* parameter);

STOMPBOX_EXTERN int GetParameterBPMSyncDenominator(void* parameter);

STOMPBOX_EXTERN void SetParameterBPMSyncNumerator(void* parameter, int numerator);

STOMPBOX_EXTERN void SetParameterBPMSyncDenominator(void* parameter, int denom);

STOMPBOX_EXTERN bool GetParameterIsAdvanced(void* parameter);

STOMPBOX_EXTERN bool GetParameterIsOutput(void* parameter);

STOMPBOX_EXTERN const char* GetParameterFilePath(void* parameter);

STOMPBOX_EXTERN const char* GetParameterDisplayFormat(void* parameter);

#ifdef __cplusplus
}
#endif