#pragma once

#include <list>

#include "StompBox.h"

#include <filesystem>
#include <unordered_map>

class PluginFactory;

typedef StompBox * (*CreatePluginFunction)(PluginFactory& factory);

class PluginFactory
{
protected:
	std::unordered_map<std::string, CreatePluginFunction> pluginMap;
	std::unordered_map<std::string, StompBox*> loadedPlugins;
	std::vector<std::string> allPluginList;
	std::filesystem::path dataBasePath;

public:
	PluginFactory();
	~PluginFactory() {}
	std::filesystem::path& GetDataPath()
	{
		return dataBasePath;
	}
	void SetDataPath(std::filesystem::path path)
	{
		dataBasePath.assign(path);
	}
	void AddPlugin(std::string name, CreatePluginFunction function);
	std::vector<std::string>& GetAllPlugins();
	void LoadAllPlugins();
	StompBox* CreatePlugin(std::string const& name, std::string const& id);
	StompBox* CreatePlugin(std::string const& id)
	{
		std::size_t found = id.find('_', 0);

		if (found != std::string::npos)
		{
			return CreatePlugin(id.substr(0, found), id);
		}

		return CreatePlugin(id, id);
	}
	void ReleasePlugin(std::string id);
	StompBox* FindPlugin(std::string id);
};

