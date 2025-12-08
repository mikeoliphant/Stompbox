#pragma once

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

StompBox* CreateInputGainPlugin(PluginFactory &factory)
{
	Gain* input = new Gain(0, -40, 40);

	input->IsUserSelectable = false;

	return input;
}

StompBox* CreateMasterVolumePlugin(PluginFactory& factory)
{
	Gain* master = new Gain(0, -40, 40);

	master->Parameters[GAIN_GAIN].Name = "Volume";

	master->IsUserSelectable = false;

	return master;
}

StompBox* CreateTonestackPlugin(PluginFactory& factory)
{
	Tonestack* tonestack = new Tonestack();
	tonestack->SetPreset(0);

	return tonestack;
}

StompBox* CreateGuitarConvolverPlugin(PluginFactory& factory)
{
	GuitarConvolver* convolver = new GuitarConvolver("Cabinets", { ".wav" }, factory.GetDataPath());

	return convolver;
}

StompBox* CreateConvolutionReverbPlugin(PluginFactory& factory)
{
	GuitarConvolver* reverb = new GuitarConvolver("Reverb", { ".wav" }, factory.GetDataPath());
	reverb->Description = "Convolution reverb (impulse response)";

	reverb->GetParameter(CONVOLVER_DRY)->SetValue(1);
	reverb->GetParameter(CONVOLVER_WET)->SetValue(0.3f);
	reverb->GetParameter(CONVOLVER_WET)->DefaultValue = 0.3f;

	reverb->SetMaxIRSamples(-1);
	reverb->SetIREpsilon(0);

	return reverb;
}

StompBox* CreateNAMPlugin(PluginFactory& factory)
{
	NAM* nam = new NAM("NAM", { ".nam", ".json", ".aidax" }, factory.GetDataPath());

	nam->InputGain = CreateInputGainPlugin(factory);
	nam->InputGain->ID = "NAM";
	nam->OutputVolume = CreateMasterVolumePlugin(factory);
	nam->OutputVolume->ID = "NAM";

	return nam;
}

StompBox* CreateNAMMultiBandPlugin(PluginFactory& factory)
{
	NAMMultiBand* nam = new NAMMultiBand("NAM", { ".nam", ".json", ".aidax" }, factory.GetDataPath());

	return nam;
}

StompBox* CreateLevelPlugin(PluginFactory& factory)
{
	Gain* level = new Gain(0, -40, 40);

	level->Parameters[GAIN_GAIN].Name = "Volume";

	return level;
}

StompBox* CreateBoostPlugin(PluginFactory& factory)
{
	Gain* gain = new Gain(10, 0, 20);
	gain->Description = "Clean boost effect";

	gain->BackgroundColor = "#e31b00";
	gain->ForegroundColor = "#ffffff";

	return gain;
}

StompBox* CreateScreamerPlugin(PluginFactory& factory)
{
	TS9* ts9 = new TS9();

	ts9->BackgroundColor = "#01b434";

	return ts9;
}

StompBox* CreateCompressorPlugin(PluginFactory& factory)
{
	Compressor* compressor = new Compressor();

	compressor->BackgroundColor = "#0043db";
	compressor->ForegroundColor = "#ffffff";

	return compressor;
}

StompBox* CreateFuzzPlugin(PluginFactory& factory)
{
	Fuzz* fuzz = new Fuzz();

	fuzz->BackgroundColor = "#fabd00";

	return fuzz;
}

StompBox* CreateAutoWahPlugin(PluginFactory& factory)
{
	AutoWah* wah = new AutoWah();

	wah->BackgroundColor = "#e00095";
	wah->ForegroundColor = "#ffffff";

	return wah;
}

StompBox* CreateWahPlugin(PluginFactory& factory)
{
	Wah* wah = new Wah();

	wah->BackgroundColor = "#111111";
	wah->ForegroundColor = "#ffffff";

	return wah;
}

StompBox* CreatePhaserPlugin(PluginFactory& factory)
{
	Phaser* phaser = new Phaser();

	phaser->BackgroundColor = "#fa4b00";

	return phaser;
}

StompBox* CreateVibratoPlugin(PluginFactory& factory)
{
	Vibrato* vibrato = new Vibrato();

	vibrato->BackgroundColor = "#066800";

	return vibrato;
}

StompBox* CreateFlangerPlugin(PluginFactory& factory)
{
	Flanger* flanger = new Flanger();

	flanger->BackgroundColor = "#94008c";
	flanger->ForegroundColor = "#ffffff";

	return flanger;
}

StompBox* CreateChorusPlugin(PluginFactory& factory)
{
	chorus::Chorus* chorus = new chorus::Chorus();

	chorus->BackgroundColor = "#00c5c5";

	return chorus;
}

StompBox* CreateTremoloPlugin(PluginFactory& factory)
{
	Tremolo* tremolo = new Tremolo();

	tremolo->BackgroundColor = "#008a60";
	tremolo->ForegroundColor = "#ffffff";

	return tremolo;
}

StompBox* CreateDelayPlugin(PluginFactory& factory)
{
	Delay* delay = new Delay();

	delay->BackgroundColor = "#ecdac6";

	return delay;
}

StompBox* CreateReverbPlugin(PluginFactory& factory)
{
	FreeVerb* reverb = new FreeVerb();

	reverb->BackgroundColor = "#006b99";
	reverb->ForegroundColor = "#ffffff";

	return reverb;
}

StompBox* CreateEQ7Plugin(PluginFactory& factory)
{
	float* frequencies = new float[7];
	float startFrequency = 100;

	for (int band = 0; band < 7; band++)
	{
		frequencies[band] = startFrequency * (float)pow(2, band);
	}

	GraphicEqualizer* eq7 = new GraphicEqualizer(7, frequencies, 1);
	eq7->Description = "7-band EQ for guitar frequencies";

	eq7->BackgroundColor = "#d6d3c1";

	return eq7;
}

StompBox* CreateBassEQ7Plugin(PluginFactory& factory)
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

StompBox* CreateHighLowFilter(PluginFactory& factory)
{
	HighLowFilter* filter = new HighLowFilter();

	return filter;
}

StompBox* CreateNoiseGatePlugin(PluginFactory& factory)
{
	NoiseGate* gate = new NoiseGate();

	gate->BackgroundColor = "#111111";
	gate->ForegroundColor = "#ffffff";

	return gate;
}

StompBox* CreateMultiGatePlugin(PluginFactory& factory)
{
	MultiGate* gate = new MultiGate();

	gate->BackgroundColor = "#111111";
	gate->ForegroundColor = "#ffffff";

	return gate;
}

StompBox* CreateTunerPlugin(PluginFactory& factory)
{
	PitchDetector* tuner = new PitchDetector(4096);

	tuner->IsUserSelectable = false;
	tuner->Enabled = false;

	return tuner;
}

StompBox* CreateAudioFilePlayerPlugin(PluginFactory& factory)
{
	AudioFilePlayer* player = new AudioFilePlayer("Music", { ".wav" }, factory.GetDataPath());

	player->IsUserSelectable = false;

	return player;
}

StompBox* CreateAudioFileRecorderPlugin(PluginFactory& factory)
{
	AudioFileRecorder* recorder = new AudioFileRecorder(factory.GetDataPath() / "Music");

	recorder->IsUserSelectable = false;

	return recorder;
}

void AddDefaultPlugins(PluginFactory& factory)
{
	factory.AddPlugin("Boost", &CreateBoostPlugin);
	factory.AddPlugin("Screamer", &CreateScreamerPlugin);
	factory.AddPlugin("Compressor", &CreateCompressorPlugin);
	factory.AddPlugin("Fuzz", &CreateFuzzPlugin);
	factory.AddPlugin("AutoWah", &CreateAutoWahPlugin);
	factory.AddPlugin("Wah", &CreateWahPlugin);
	factory.AddPlugin("Phaser", &CreatePhaserPlugin);
	factory.AddPlugin("Vibrato", &CreateVibratoPlugin);
	factory.AddPlugin("Flanger", &CreateFlangerPlugin);
	factory.AddPlugin("Chorus", &CreateChorusPlugin);
	factory.AddPlugin("Tremolo", &CreateTremoloPlugin);
	factory.AddPlugin("Delay", &CreateDelayPlugin);
	factory.AddPlugin("Reverb", &CreateReverbPlugin);
	factory.AddPlugin("ConvoReverb", &CreateConvolutionReverbPlugin);
	factory.AddPlugin("NAM", &CreateNAMPlugin);
	factory.AddPlugin("NAMMulti", &CreateNAMMultiBandPlugin);
	factory.AddPlugin("EQ-7", &CreateEQ7Plugin);
	factory.AddPlugin("BEQ-7", &CreateBassEQ7Plugin);
	factory.AddPlugin("HighLow", &CreateHighLowFilter);
	factory.AddPlugin("NoiseGate", &CreateMultiGatePlugin);
	factory.AddPlugin("SimpleGate", &CreateNoiseGatePlugin);
	factory.AddPlugin("Cabinet", &CreateGuitarConvolverPlugin);
	factory.AddPlugin("Level", &CreateLevelPlugin);
	factory.AddPlugin("Input", &CreateInputGainPlugin);
	factory.AddPlugin("Master", &CreateMasterVolumePlugin);
	factory.AddPlugin("Tuner", &CreateTunerPlugin);
	factory.AddPlugin("AudioFilePlayer", &CreateAudioFilePlayerPlugin);
	factory.AddPlugin("AudioFileRecorder", &CreateAudioFileRecorderPlugin);
}