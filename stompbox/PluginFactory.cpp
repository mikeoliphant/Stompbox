#include <iostream>
#include <filesystem>

#include "PluginFactory.h"


PluginFactory::PluginFactory()
{
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

		component = func(*this);

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

