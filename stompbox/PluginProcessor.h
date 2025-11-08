#pragma once

#include <math.h>
#include <list>
#include "StompBox.h"
#include "PluginFactory.h"
#include "StompboxServer.h"
#include "SerialDisplayInterface.h"
#include "DoubleBufferedResource.h"

enum
{
	MIDI_MODE_STOMP,
	MIDI_MODE_PRESETCHANGE,
	MIDI_MODE_TUNER,
	MIDI_MODE_RECORDER,
	MIDI_MODE_MAX
};

#define PRESET_TEXT_COLOR "#FFFFFF"
#define PRESET_PREVIEW_COLOR "#FFAA00"

struct CCMap
{
	int CCNumber;
	StompBox* Plugin;
	std::string Parameter;
};

class ChainElement
{
public:
	std::string Name;
	bool IsChain = false;
	bool IsMaster = false;
	bool IsSplit = false;
	DoubleBufferedResource<std::vector<StompBox*>> Plugins;
};

class PluginProcessor
{
protected:
	void UpdateClient();
	void SendClientMessage(const std::string message);
	void AppendParamDefs(std::string& dump, StompBox* plugin);
	void AppendParamDef(std::string& dump, StompBoxParameter* param);
	void AppendPluginParams(std::string& dump, StompBox* plugin, bool dirtyOnly);
	void AppendParams(std::string& dump, StompBox* plugin, bool dirtyOnly);
	void ThreadLoadPreset();

	float sampleRate;
	float bpm = 120;
	int tmpBufSize = 64;
	float* tmpBuf1 = nullptr;
	float* tmpBuf2 = nullptr;
	bool initialized = false;
	PluginFactory pluginFactory;
	StompboxServer stompboxServer;
	SerialDisplayInterface serialDisplayInterface;
	std::unordered_map<std::string, ChainElement*> chainLookup;
	std::vector<ChainElement*> chainList;
	std::vector<std::string> presets;
	StompBox* tuner = nullptr;
	StompBox* audioFileRecorder = nullptr;
	StompBox* monitorPlugin = nullptr;
	std::function<void(float*, int)> monitorCallback = nullptr;
	std::function<void(int, int, int)> midiCallback = nullptr;
	std::function<void(const std::string)> messageCallback = nullptr;
	bool serverIsRunning = false;
	std::thread* clientUpdateThread = nullptr;
	std::filesystem::path dataPath;
	std::filesystem::path presetPath;
	std::string currentPreset;
	std::string loadPreset;
	std::string previewPreset;
	std::string clientCommand;
	bool pluginOutputOn = false;
	bool midiLocked = false;
	int currentMidiMode = MIDI_MODE_STOMP;
	std::list<CCMap> ccMapEntries;
	int modeChangeCC = -1;
	int stompCC[16];
	float minDSPLoad = 1;
	float maxDSPLoad = 0;
	int clientUpdateFrame = 0;
	std::thread* presetLoadThread = nullptr;
	int ramp = 0;
	int rampSamplesSoFar;
	int rampSamples;
	float defaultRampMS = 25;
	bool needPluginUpdate = false;
	std::vector<float> splitBuffer;

public:
	PluginProcessor(std::filesystem::path dataPath, bool dawMode);
	~PluginProcessor();
	static std::string GetVersion();
	void ScanPresets();
	void Init(float sampleRate);
	void SetMaxAudioBufferSize(size_t numSamples);
	void SetBPM(float bpm);
	void StartServer();
	StompBox* CreatePlugin(std::string const& id);
	void UpdatePlugins();
	void InitPlugin(StompBox* plugin);

	bool IsPresetLoading()
	{
		return (ramp != 0);
	}

	float GetBPM()
	{
		return bpm;
	}

	std::filesystem::path& GetDataPath()
	{
		return dataPath;
	}

	bool GetServerIsRunning() {
		return serverIsRunning;
	}

	bool HaveClient()
	{
		if (!serverIsRunning)
			return false;

		return stompboxServer.HaveClient();
	}

	PluginFactory* GetPluginFactory()
	{
		return &pluginFactory;
	}

	std::string GetGlobalChain()
	{
		std::string chainStr;

		for (auto& element : chainList)
		{
			if (element->IsMaster)
				chainStr.append("Master");

			if (element->IsSplit)
				chainStr.append("Split");

			if (element->IsChain)
				chainStr.append("Chain ");
			else
				chainStr.append("Slot ");

			chainStr.append(element->Name);

			chainStr.append(" ");
		}

		return chainStr;
	}

	StompBox* GetPluginSlot(const std::string& slotName)
	{
		auto slot = chainLookup.find(slotName);

		if (slot != chainLookup.end())
		{
			if (slot->second->IsChain)
				return nullptr;

			auto& plugins = slot->second->Plugins.GetRead();

			if (plugins.size() == 0)
				return nullptr;

			return plugins[0];
		}

		return nullptr;
	}

	void SetPluginSlot(const std::string& slotName, const std::string& pluginID)
	{
		std::cout << "Set Plugin Slot: " << slotName << " to " << pluginID << std::endl;

		auto slot = chainLookup.find(slotName);

		if (slot != chainLookup.end())
		{
			if (!slot->second->IsChain)
			{
				auto plugin = CreatePlugin(pluginID);

				if (plugin != nullptr)
				{
					auto& plugins = slot->second->Plugins.GetWriteLock();

					plugins.clear();
					plugins.push_back(CreatePlugin(pluginID));

					slot->second->Plugins.FinishWrite();
				}
			}
		}
	}

	ChainElement *GetChain(const std::string& chainName)
	{
		auto chain = chainLookup.find(chainName);

		if (chain != chainLookup.end())
		{
			return chain->second;
		}

		return nullptr;
	}

	std::vector<std::string>& GetPresets()
	{
		return presets;
	}

	std::string& GetCurrentPreset()
	{
		return currentPreset;
	}

	std::string GetNextPreset(std::string afterPreset);
	std::string GetPreviousPreset(std::string beforePreset);

	void SetMonitorPlugin(std::string const& pluginID, std::function<void(float *, int)> callback)
	{
		monitorCallback = callback;
		monitorPlugin = pluginFactory.FindPlugin(pluginID);
	}

	void SetMidiCallback(std::function<void(int, int, int)> callback)
	{
		midiCallback = callback;
	}

	void SetMessageCallback(std::function<void(const std::string)> callback);
	void ReportDSPLoad(float currentLoad);
	std::string HandleCommand(const std::string& line);
	bool HandleMidiCommand(int midiCommand, int midiData1, int midiData2);
	bool CheckMidiCommand(StompBox* plugin, StompBoxParameter* parameter);
	void SyncPreset();
	void LoadPreset(std::string preset);
	void LoadSettings();
	void SaveSettings();
	bool LoadCommandsFromFile(std::filesystem::path filePath);
	std::string DumpSettings();
	std::string DumpProgram();
	std::string DumpConfig();
	void Process(float* input, float* output, size_t count);
	void StartRamp(int rampDirection, float rampMS);
};