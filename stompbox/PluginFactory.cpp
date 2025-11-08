#include <iostream>
#include <filesystem>

#include "PluginFactory.h"

#include "Gain.h"
#include "GuitarConvolver.h"
#include "ToneStack.h"
#include "PitchDetector.h"
#include "AudioFilePlayer.h"
#include "AudioFileRecorder.h"
#include "NAM.h"
#include "NAMMultiBand.h"
#include "Tremolo.h"
#include "AutoWah.h"
#include "Wah.h"
#include "Delay.h"
#include "Reverb.h"
#include "Compressor.h"
#include "Phaser.h"
#include "Vibrato.h"
#include "Flanger.h"
#include "Chorus.h"
#include "Fuzz.h"
#include "GraphicEqualizer.h"
#include "HighLowFilter.h"
#include "NoiseGate.h"
#include "MultiGate.h"
#include "Screamer.h"

std::filesystem::path dataBasePath;

StompBox* CreateInputGainPlugin()
{
	Gain *input = new Gain(0, -40, 40);

	input->IsUserSelectable = false;

	return input;
}

StompBox* CreateMasterVolumePlugin()
{
	Gain* master = new Gain(0, -40, 40);

	master->Parameters[GAIN_GAIN].Name = "Volume";

	master->IsUserSelectable = false;

	return master;
}

StompBox* CreateTonestackPlugin()
{
	Tonestack* tonestack = new Tonestack();
	tonestack->SetPreset(0);

	return tonestack;
}

std::filesystem::path cabinetPath;

StompBox* CreateGuitarConvolverPlugin()
{
	GuitarConvolver* convolver = new GuitarConvolver("Cabinets", { ".wav" }, dataBasePath);

	return convolver;
}

std::filesystem::path reverbPath;

StompBox* CreateConvolutionReverbPlugin()
{
	GuitarConvolver* reverb = new GuitarConvolver("Reverb", { ".wav" }, dataBasePath);
	reverb->Description = "Convolution reverb (impulse response)";

	reverb->GetParameter(CONVOLVER_DRY)->SetValue(1);
	reverb->GetParameter(CONVOLVER_WET)->SetValue(0.3f);
	reverb->GetParameter(CONVOLVER_WET)->DefaultValue = 0.3f;

	reverb->SetMaxIRSamples(-1);
	reverb->SetIREpsilon(0);

	return reverb;
}

std::filesystem::path namModelPath;

StompBox* CreateNAMPlugin()
{
	NAM* nam = new NAM("NAM", { ".nam", ".json", ".aidax" }, dataBasePath);

	nam->InputGain = CreateInputGainPlugin();
	nam->InputGain->ID = "NAM";
	nam->OutputVolume = CreateMasterVolumePlugin();
	nam->OutputVolume->ID = "NAM";

	return nam;
}

StompBox* CreateNAMMultiBandPlugin()
{
	NAMMultiBand* nam = new NAMMultiBand("NAM", { ".nam", ".json", ".aidax" }, dataBasePath);

	return nam;
}

StompBox* CreateLevelPlugin()
{
	Gain* level = new Gain(0, -40, 40);

	level->Parameters[GAIN_GAIN].Name = "Volume";

	return level;
}

StompBox* CreateBoostPlugin()
{
	Gain *gain = new Gain(10, 0, 20);
	gain->Description = "Clean boost effect";

	gain->BackgroundColor = "#e31b00";
	gain->ForegroundColor = "#ffffff";

	return gain;
}

StompBox* CreateScreamerPlugin()
{
	TS9* ts9 = new TS9();

	ts9->BackgroundColor = "#01b434";

	return ts9;
}

StompBox* CreateCompressorPlugin()
{
	Compressor *compressor = new Compressor();

	compressor->BackgroundColor = "#0043db";
	compressor->ForegroundColor = "#ffffff";

	return compressor;
}

StompBox* CreateFuzzPlugin()
{
	Fuzz *fuzz = new Fuzz();

	fuzz->BackgroundColor = "#fabd00";

	return fuzz;
}

StompBox* CreateAutoWahPlugin()
{
	AutoWah *wah = new AutoWah();

	wah->BackgroundColor = "#e00095";
	wah->ForegroundColor = "#ffffff";

	return wah;
}

StompBox* CreateWahPlugin()
{
	Wah* wah = new Wah();

	wah->BackgroundColor = "#111111";
	wah->ForegroundColor = "#ffffff";

	return wah;
}

StompBox* CreatePhaserPlugin()
{
	Phaser *phaser = new Phaser();

	phaser->BackgroundColor = "#fa4b00";

	return phaser;
}

StompBox* CreateVibratoPlugin()
{
	Vibrato *vibrato = new Vibrato();

	vibrato->BackgroundColor = "#066800";

	return vibrato;
}

StompBox* CreateFlangerPlugin()
{
	Flanger *flanger = new Flanger();

	flanger->BackgroundColor = "#94008c";
	flanger->ForegroundColor = "#ffffff";

	return flanger;
}

StompBox* CreateChorusPlugin()
{
	chorus::Chorus *chorus = new chorus::Chorus();

	chorus->BackgroundColor = "#00c5c5";

	return chorus;
}

StompBox* CreateTremoloPlugin()
{
	Tremolo *tremolo = new Tremolo();

	tremolo->BackgroundColor = "#008a60";
	tremolo->ForegroundColor = "#ffffff";

	return tremolo;
}

StompBox* CreateDelayPlugin()
{
	Delay*delay = new Delay();

	delay->BackgroundColor = "#ecdac6";

	return delay;
}

StompBox* CreateReverbPlugin()
{
	FreeVerb *reverb = new FreeVerb();

	reverb->BackgroundColor = "#006b99";
	reverb->ForegroundColor = "#ffffff";

	return reverb;
}

StompBox* CreateEQ7Plugin()
{
	float* frequencies = new float[7];
	float startFrequency = 100;

	for (int band = 0; band < 7; band++)
	{
		frequencies[band] = startFrequency * (float)pow(2, band);
	}

	GraphicEqualizer *eq7 = new GraphicEqualizer(7, frequencies, 1);
	eq7->Description = "7-band EQ for guitar frequencies";

	eq7->BackgroundColor = "#d6d3c1";

	return eq7;
}

StompBox* CreateBassEQ7Plugin()
{
	float* frequencies = new float[7];

	frequencies[0] = 50;
	frequencies[1] = 120;
	frequencies[2] = 400;
	frequencies[3] = 500;
	frequencies[4] = 800;
	frequencies[5] = 4500;
	frequencies[6] = 10000;


	GraphicEqualizer* beq7 = new GraphicEqualizer(7, frequencies, 1);
	beq7->Description = "7-band EQ for bass frequencies";

	beq7->BackgroundColor = "#a0a0a0";

	return beq7;
}

StompBox* CreateHighLowFilter()
{
	HighLowFilter* filter = new HighLowFilter();

	return filter;
}

StompBox* CreateNoiseGatePlugin()
{
	NoiseGate* gate = new NoiseGate();

	gate->BackgroundColor = "#111111";
	gate->ForegroundColor = "#ffffff";

	return gate;
}

StompBox* CreateMultiGatePlugin()
{
	MultiGate* gate = new MultiGate();

	gate->BackgroundColor = "#111111";
	gate->ForegroundColor = "#ffffff";

	return gate;
}

StompBox* CreateTunerPlugin()
{
	PitchDetector *tuner = new PitchDetector(4096);

	tuner->IsUserSelectable = false;
	tuner->Enabled = false;

	return tuner;
}

StompBox* CreateAudioFilePlayerPlugin()
{
	AudioFilePlayer* player = new AudioFilePlayer("Music", { ".wav" }, dataBasePath);

	player->IsUserSelectable = false;

	return player;
}

StompBox* CreateAudioFileRecorderPlugin()
{
	AudioFileRecorder* recorder = new AudioFileRecorder(dataBasePath / "Music");

	recorder->IsUserSelectable = false;

	return recorder;
}


PluginFactory::PluginFactory()
{
	AddPlugin("Boost", &CreateBoostPlugin);
	AddPlugin("Screamer", &CreateScreamerPlugin);
	AddPlugin("Compressor", &CreateCompressorPlugin);
	AddPlugin("Fuzz", &CreateFuzzPlugin);
	AddPlugin("AutoWah", &CreateAutoWahPlugin);
	AddPlugin("Wah", &CreateWahPlugin);
	AddPlugin("Phaser", &CreatePhaserPlugin);
	AddPlugin("Vibrato", &CreateVibratoPlugin);
	AddPlugin("Flanger", &CreateFlangerPlugin);
	AddPlugin("Chorus", &CreateChorusPlugin);
	AddPlugin("Tremolo", &CreateTremoloPlugin);
	AddPlugin("Delay", &CreateDelayPlugin);
	AddPlugin("Reverb", &CreateReverbPlugin);
	AddPlugin("ConvoReverb", &CreateConvolutionReverbPlugin);
	AddPlugin("NAM", &CreateNAMPlugin);
	AddPlugin("NAMMulti", &CreateNAMMultiBandPlugin);
	AddPlugin("EQ-7", &CreateEQ7Plugin);
	AddPlugin("BEQ-7", &CreateBassEQ7Plugin);
	AddPlugin("HighLow", &CreateHighLowFilter);
	AddPlugin("NoiseGate", &CreateMultiGatePlugin);
	AddPlugin("SimpleGate", &CreateNoiseGatePlugin);
	AddPlugin("Cabinet", &CreateGuitarConvolverPlugin);
	AddPlugin("Level", &CreateLevelPlugin);
	AddPlugin("Input", &CreateInputGainPlugin);
	AddPlugin("Master", &CreateMasterVolumePlugin);
	AddPlugin("Tuner", &CreateTunerPlugin);
	AddPlugin("AudioFilePlayer", &CreateAudioFilePlayerPlugin);
	AddPlugin("AudioFileRecorder", &CreateAudioFileRecorderPlugin);
}

void PluginFactory::AddPlugin(std::string name, CreatePluginFunction function)
{
	pluginMap[name] = function;

	allPluginList.push_back(name);
}

std::vector<std::string>& PluginFactory::GetAllPlugins()
{
	return allPluginList;
}

void PluginFactory::LoadAllPlugins()
{
	for (auto kv : pluginMap)
	{
		CreatePlugin(kv.first, kv.first);
	}
}


StompBox* PluginFactory::CreatePlugin(std::string const& name, std::string const& id)
{
	StompBox* component = FindPlugin(id);

	if (component != nullptr)
	{
		fprintf(stderr, "Plugin %s already loaded\n", id.c_str());

		return component;
	}

	auto iter = pluginMap.find(name);
	if (iter != pluginMap.end())
	{
		CreatePluginFunction func = iter->second;

		component = func();

		component->Name.assign(name);
		component->ID.assign(id);

		loadedPlugins[component->ID] = component;

		fprintf(stderr, "Loaded Plugin %s\n", id.c_str());

		return component;
	}

	fprintf(stderr, "Plugin not found %s\n", id.c_str());

	return nullptr;
}

void PluginFactory::ReleasePlugin(std::string id)
{
	StompBox* component = FindPlugin(id);

	if (component != nullptr)
	{
		loadedPlugins.erase(id);

		fprintf(stderr, "Releasing Plugin %s\n", id.c_str());
	}
}

StompBox* PluginFactory::FindPlugin(std::string id)
{
	auto iter = loadedPlugins.find(id);
	if (iter != loadedPlugins.end())
	{
		return iter->second;
	}

	return nullptr;
}

