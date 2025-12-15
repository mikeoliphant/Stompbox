#include "StompboxLV2Plugin.h"

extern StompboxLV2PluginFactory* GetLV2PluginFactory();

StompboxLV2PluginFactory* factory = GetLV2PluginFactory();

void connect_port(LV2_Handle instance, uint32_t port, void* data)
{
	auto plugin = static_cast<StompboxLV2Plugin*>(instance);

	plugin->SetPortDataPtr(port, data);
}

void activate(LV2_Handle) {}

void run(LV2_Handle instance, uint32_t n_samples)
{
	auto plugin = static_cast<StompboxLV2Plugin*>(instance);

	plugin->Process(n_samples);
}

void deactivate(LV2_Handle instance) {}

void cleanup(LV2_Handle instance)
{
	auto plugin = static_cast<StompboxLV2Plugin*>(instance);

	delete plugin;
}

const void* extension_data(const char* uri)
{
	return nullptr;
}

LV2_SYMBOL_EXPORT const LV2_Descriptor* lv2_descriptor(uint32_t index)
{
	return factory->GetDescriptor(index);
}
