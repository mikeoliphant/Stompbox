#include "SimpleIO.h"

StompboxLV2PluginFactory* GetLV2PluginFactory()
{
	return new SimpleIOPluginFactory();
}

