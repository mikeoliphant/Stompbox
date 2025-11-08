#pragma once

#include <list>

#include "StompBox.h"

#include <filesystem>
#include <unordered_map>

typedef StompBox * (*CreatePluginFunction)(void);

extern std::filesystem::path dataBasePath;
extern std::filesystem::path cabinetPath;
extern std::filesystem::path reverbPath;
extern std::filesystem::path namModelPath;

class PluginFactory
{
protected:
	std::unordered_map<std::string, CreatePluginFunction> pluginMap;
	std::unordered_map<std::string, StompBox*> loadedPlugins;
	std::vector<std::string> allPluginList;

	void AddPlugin(std::string name, CreatePluginFunction function);

public:
	PluginFactory();
	~PluginFactory() {}
	void SetDataPath(std::filesystem::path path)
	{
		dataBasePath.assign(path);

		cabinetPath.assign(path);
		cabinetPath.append("Cabinets");

		if (!std::filesystem::exists(cabinetPath))
			std::filesystem::create_directory(cabinetPath);

		reverbPath.assign(path);
		reverbPath.append("Reverb");

		if (!std::filesystem::exists(reverbPath))
			std::filesystem::create_directory(reverbPath);

		namModelPath.assign(path);
		namModelPath.append("NAM");

		if (!std::filesystem::exists(namModelPath))
			std::filesystem::create_directory(namModelPath);
	}
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

