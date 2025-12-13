#pragma once

#include "StompboxLV2Plugin.h"
#include "StompBox.h"
#include "Gain.h"

StompBox* CreateInputGainPlugin(PluginFactory& factory)
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

class SimpleIOPlugin : public StompboxLV2Plugin
{
	public:	
		SimpleIOPlugin() :
			inputGainPort(),
			outputVolumePort()
		{
			processor.GetPluginFactory().AddPlugin("Gain", &CreateInputGainPlugin);
			processor.GetPluginFactory().AddPlugin("Volume", &CreateMasterVolumePlugin);

			processor.HandleCommand("SetGlobalChain MasterChain Chain");
			processor.HandleCommand("SetChain Chain Gain Volume");

			AddPort(inputGainPort);
			inputGainPort.SetParameter(processor.CreatePlugin("Gain")->GetParameter("Gain"));

			AddPort(outputVolumePort);
			outputVolumePort.SetParameter(processor.CreatePlugin("Volume")->GetParameter("Volume"));
		}

	private:
		StompboxLV2ControlPort inputGainPort;
		StompboxLV2ControlPort outputVolumePort;
};

class SimpleIOPluginFactory : public StompboxLV2PluginFactory
{
	public:
		SimpleIOPluginFactory()
		{
			AddPlugin<SimpleIOPlugin>("https://github.com/mikeoliphant/simple_io");
		}
};
