#include <iostream>
#include <fstream>
#include <sstream>

#include "architecture.hpp"

#include "PluginProcessor.h"
#include "AudioFileRecorder.h"

#ifdef __linux__
#include <cstdlib>
#endif

#if _WIN32
void GetDataPathFromModule(std::filesystem::path& dataPath)
{
    char path[MAX_PATH];

    HMODULE hm = NULL;

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&GetDataPathFromModule, &hm) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }
    if (GetModuleFileNameA(hm, path, MAX_PATH) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }

    size_t len = strlen(path);

    for (size_t i = len - 1; i > 0; i--)
    {
        if ((path[i] == '/') || (path[i] == '\\'))
        {
            path[i] = 0;

            break;
        }
    }

    dataPath.assign(path);
}
#endif

std::string PluginProcessor::GetVersion()
{
    return "stompbox v0.1.15 - Copyright 2023-2025 Mike Oliphant";
}

PluginProcessor::PluginProcessor(std::filesystem::path dataPath, bool dawMode)
{
    this->dataPath.assign(dataPath);

    presetPath.assign(dataPath);
    presetPath.append("Presets");    

    if (!std::filesystem::exists(presetPath))
        std::filesystem::create_directory(presetPath);

    ScanPresets();

    pluginFactory.SetDataPath(dataPath);

    tmpBuf1 = new float[tmpBufSize];
    tmpBuf2 = new float[tmpBufSize];

    for (int i = 0; i < tmpBufSize; i++)
    {
        tmpBuf1[i] = 0;
    }

    for (int i = 0; i < 16; i++)
    {
        stompCC[i] = -1;
    }

    needPluginUpdate = true;

    LoadSettings();

    serialDisplayInterface.Connect();

    //StartServer();

    //StompboxClient.SetLineCallback([this](std::string line) { return HandleCommand(line); });
    //StompboxClient.Start();
}

PluginProcessor::~PluginProcessor()
{
    if (tmpBuf1 != nullptr)
    {
        delete[] tmpBuf1;
        delete[] tmpBuf2;
    }

    if (serialDisplayInterface.IsConnected())
        serialDisplayInterface.Close();
}

void PluginProcessor::Init(float newSampleRate)
{
    fprintf(stderr, "Init with sample rate: %d\n", (int)newSampleRate);

    sampleRate = newSampleRate;

    bool wasInitialized = initialized;
    initialized = true;

    for (auto& element : chainList)
    {
        for (auto& plugin : element->Plugins.GetRead())
        {
            // Initialize right now if it is the first time. Otherwise we could be running, so set a flag to initialize in Process()
            if (!wasInitialized)
                InitPlugin(plugin);
            else
                plugin->NeedsInit = true;
        }
    }

    initialized = true;

    fprintf(stderr, "Starting client update thread\n");

    clientUpdateThread = new std::thread(&PluginProcessor::UpdateClient, this);
}

void PluginProcessor::SetMaxAudioBufferSize(size_t numSamples)
{
    splitBuffer.resize(numSamples);
}

void PluginProcessor::ScanPresets()
{
    presets.clear();

    for (const auto& entry : std::filesystem::directory_iterator(presetPath))
    {
        presets.push_back(entry.path().filename().string());
    }
}

void PluginProcessor::SetBPM(float newBpm)
{
    bpm = newBpm;

    for (auto& element : chainList)
    {
        for (auto& plugin : element->Plugins.GetRead())
        {
            plugin->SetBPM(bpm);
        }
    }
}

void PluginProcessor::StartServer()
{
    stompboxServer.SetLineCallback([this](std::string line) { return HandleCommand(line); });
    stompboxServer.Start();

    serverIsRunning = true;
}

void PluginProcessor::SetMessageCallback(std::function<void(const std::string)> callback)
{
    messageCallback = callback;

    if (clientUpdateThread == nullptr)
    {
        clientUpdateThread = new std::thread(&PluginProcessor::UpdateClient, this);
    }
}

void PluginProcessor::ReportDSPLoad(float currentLoad)
{
    if (currentLoad < minDSPLoad)
        minDSPLoad = currentLoad;

    if (currentLoad > maxDSPLoad)
        maxDSPLoad = currentLoad;
}

void PluginProcessor::UpdateClient()
{
    std::string cmd;

    do
    {
        if (!(HaveClient() && pluginOutputOn) && (messageCallback == nullptr) && !serialDisplayInterface.IsConnected())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (serialDisplayInterface.IsConnected())
            {
                switch (currentMidiMode)
                {
                    case MIDI_MODE_TUNER:
                    {
                        if ((tuner != nullptr) && (tuner->Enabled && (tuner->GetParameter("Pitch") != nullptr)))
                        {
                            serialDisplayInterface.UpdateTuner(tuner->GetParameter("Pitch")->GetValue());
                        }

                        break;
                    }

                    case MIDI_MODE_RECORDER:
                    {
                        if ((audioFileRecorder != nullptr) && audioFileRecorder->Enabled)
                        {
                            float secondsRecorded = audioFileRecorder->GetParameterValue(AUDIOFILERECORDER_SECONDSRECORDED);

                            serialDisplayInterface.UpdateRecordSeconds(secondsRecorded);
                        }

                        break;
                    }
                }
            }

            if ((HaveClient() || (messageCallback != nullptr)) && pluginOutputOn)
            {
                clientUpdateFrame++;

                for (auto& element : chainList)
                {
                    for (auto& plugin : element->Plugins.GetRead())
                    {
                        if (plugin->Enabled)
                        {
                            for (size_t i = 0; i < plugin->NumParameters; i++)
                            {
                                StompBoxParameter* param = plugin->GetParameter(i);

                                if (param->IsOutput)
                                {
                                    cmd.clear();

                                    cmd.append("SetParam ");
                                    cmd.append(plugin->ID);
                                    cmd.append(" ");
                                    cmd.append(param->Name);
                                    cmd.append(" ");
                                    cmd.append(std::to_string(param->GetValue()));
                                    cmd.append("\r\n");

                                    SendClientMessage(cmd);
                                }
                            }
                        }

                        if (plugin->ParamIsDirty || plugin->EnabledIsDirty)
                        {
                            cmd.clear();

                            AppendPluginParams(cmd, plugin, true);

                            plugin->ParamIsDirty = false;
                            plugin->EnabledIsDirty = false;

                            SendClientMessage(cmd);
                        }
                    }
                }

                if (((clientUpdateFrame % 10) == 0) && (maxDSPLoad != 0))
                {
                    SendClientMessage("DSPLoad " + std::to_string(maxDSPLoad) + " " + std::to_string(minDSPLoad) + "\r\n");

                    minDSPLoad = 1;
                    maxDSPLoad = 0;
                }
            }
        }
    }
    while (true);
}

void PluginProcessor::SendClientMessage(const std::string message)
{
    if (messageCallback != nullptr)
        messageCallback(message);

    if (HaveClient())
        stompboxServer.SendData(message);
}

void PluginProcessor::UpdatePlugins()
{
    needPluginUpdate = false;

    fprintf(stderr, "Update Plugins\n");

    for (auto& element : chainList)
    {
        for (auto& plugin : element->Plugins.GetRead())
        {
            if (plugin->NeedsInit)
            {
                InitPlugin(plugin);
            }

            fprintf(stderr, "%s ", plugin->ID.c_str());
        }
    }

    fprintf(stderr, "\n");

	tuner = pluginFactory.FindPlugin("Tuner");
    audioFileRecorder = pluginFactory.FindPlugin("AudioFileRecorder");
}

StompBox* PluginProcessor::CreatePlugin(std::string const& id)
{
    StompBox* plugin = pluginFactory.CreatePlugin(id);

    if (plugin == nullptr)
        return nullptr;

    plugin->MidiCallback = midiCallback;

    InitPlugin(plugin);

    return plugin;
}

void PluginProcessor::InitPlugin(StompBox* plugin)
{
    if (initialized)
    {
        fprintf(stderr, "Init plugin: %s\n", plugin->Name.c_str());

        plugin->init((int)sampleRate);
        plugin->SetBPM(bpm);

        if (plugin->InputGain != nullptr)
            InitPlugin(plugin->InputGain);

        if (plugin->OutputVolume != nullptr)
            InitPlugin(plugin->OutputVolume);

        plugin->NeedsInit = false;
    }
}

std::string PluginProcessor::DumpConfig()
{
    std::string dump;

    for (const auto& pluginName : pluginFactory.GetAllPlugins())
    {
        StompBox* plugin = pluginFactory.CreatePlugin(pluginName, pluginName);

        dump.append("PluginConfig ");
        dump.append(plugin->Name);

        if (!plugin->BackgroundColor.empty())
        {
            dump.append(" BackgroundColor ");
            dump.append(plugin->BackgroundColor);
        }

        if (!plugin->ForegroundColor.empty())
        {
            dump.append(" ForegroundColor ");
            dump.append(plugin->ForegroundColor);
        }

        dump.append(" IsUserSelectable ");
        dump.append(plugin->IsUserSelectable ? "1" : "0");

        if (!plugin->Description.empty())
        {
            dump.append(" Description ");

            dump.append("\"");
            dump.append(plugin->Description);
            dump.append("\"");
        }

        dump.append("\r\n");

        if (plugin->InputGain != nullptr)
            AppendParamDef(dump, plugin->InputGain->GetParameter(0));

        AppendParamDefs(dump, plugin);

        if (plugin->OutputVolume != nullptr)
            AppendParamDef(dump, plugin->OutputVolume->GetParameter(0));

        dump.append("EndConfig\r\n");
    }

    return dump;
}

void PluginProcessor::AppendParamDefs(std::string& dump, StompBox* plugin)
{
    for (size_t i = 0; i < plugin->NumParameters; i++)
    {
        AppendParamDef(dump, plugin->GetParameter(i));
    }
}

void PluginProcessor::AppendParamDef(std::string& dump, StompBoxParameter* param)
{
    dump.append("ParameterConfig ");
    dump.append(param->Stomp->ID);
    dump.append(" ");
    dump.append(param->Name);

    dump.append(" Type ");

    switch (param->ParameterType)
    {
    case PARAMETER_TYPE_KNOB:
        dump.append("Knob");
        break;
    case PARAMETER_TYPE_BOOL:
        dump.append("Bool");
        break;
    case PARAMETER_TYPE_INT:
        dump.append("Int");
        break;
    case PARAMETER_TYPE_VSLIDER:
        dump.append("VSlider");
        break;
    case PARAMETER_TYPE_ENUM:
        dump.append("Enum");
        break;
    case PARAMETER_TYPE_FILE:
        dump.append("File");
        break;
    case PARAMETER_TYPE_POWER:
        dump.append("Power");
        break;
    }

    dump.append(" MinValue ");
    dump.append(std::to_string(param->MinValue));

    dump.append(" MaxValue ");
    dump.append(std::to_string(param->MaxValue));

    dump.append(" DefaultValue ");
    dump.append(std::to_string(param->DefaultValue));

    dump.append(" RangePower ");
    dump.append(std::to_string(param->RangePower));

    dump.append(" ValueFormat ");
    dump.append(param->DisplayFormat);

    dump.append(" CanSyncToHostBPM ");
    dump.append(param->CanSyncToHostBPM ? "1" : "0");

    dump.append(" IsAdvanced ");
    dump.append(param->IsAdvanced ? "1" : "0");

    dump.append(" IsOutput ");
    dump.append(param->IsOutput ? "1" : "0");

    if (!param->Description.empty())
    {
        dump.append(" Description ");

        dump.append("\"");
        dump.append(param->Description);
        dump.append("\"");
    }

    dump.append("\r\n");

    if (param->ParameterType == PARAMETER_TYPE_ENUM)
    {
        dump.append("ParameterEnumValues ");

        dump.append(param->Stomp->Name);
        dump.append(" ");
        dump.append(param->Name);


        for (const auto& enumValue : *(param->EnumValues))
        {
            dump.append(" ");
            dump.append(enumValue);
        }

        dump.append("\r\n");
    }
    else if (param->ParameterType == PARAMETER_TYPE_FILE)
    {
        dump.append("ParameterFileTree ");

        dump.append(param->Stomp->Name);
        dump.append(" ");
        dump.append(param->Name);
        dump.append(" ");
        dump.append(param->FilePath);
        dump.append(" ");

        for (const auto& enumValue : *(param->EnumValues))
        {
            dump.append(" \"");
            dump.append(enumValue);
            dump.append("\"");
        }

        dump.append("\r\n");
    }
}

void PluginProcessor::AppendPluginParams(std::string& dump, StompBox* plugin, bool dirtyOnly)
{
    if (!dirtyOnly || plugin->EnabledIsDirty)
    {
        dump.append("SetParam ");
        dump.append(plugin->ID);
        dump.append(" Enabled ");
        dump.append(plugin->Enabled ? "1" : "0");
        dump.append("\r\n");
    }

    if (plugin->InputGain != nullptr)
    {
        AppendParams(dump, plugin->InputGain, dirtyOnly);
    }

    if (plugin->OutputVolume != nullptr)
    {
        AppendParams(dump, plugin->OutputVolume, dirtyOnly);
    }

    AppendParams(dump, plugin, dirtyOnly);
}

void PluginProcessor::AppendParams(std::string& dump, StompBox* plugin, bool dirtyOnly)
{
    for (size_t i = 0; i < plugin->NumParameters; i++)
    {
        StompBoxParameter* param = plugin->GetParameter(i);

        if (!param->SuppressSave && (!dirtyOnly || param->IsDirty))
        {
            dump.append("SetParam ");
            dump.append(plugin->ID);
            dump.append(" ");
            dump.append(param->Name);
            dump.append(" ");

            if (param->ParameterType == PARAMETER_TYPE_ENUM)
            {
                int enumIndex = (int)plugin->GetParameterValue(i);

                if ((enumIndex >= 0) && (enumIndex <= param->MaxValue))
                    dump.append((*param->EnumValues)[enumIndex]);
            }
            else if (param->ParameterType == PARAMETER_TYPE_FILE)
            {
                int enumIndex = (int)plugin->GetParameterValue(i);

                if ((enumIndex >= 0) && (enumIndex <= param->MaxValue))
                {
                    dump.append("\"");
                    dump.append((*param->EnumValues)[enumIndex]);
                    dump.append("\"");
                }
            }
            else if ((param->ParameterType == PARAMETER_TYPE_INT) || (param->ParameterType == PARAMETER_TYPE_BOOL))
            {
                dump.append(std::to_string((int)plugin->GetParameterValue(i)));
            }
            else if (param->CanSyncToHostBPM && (param->BPMSyncNumerator != 0) && (param->BPMSyncDenominator != 0))
            {
                dump.append(std::to_string(param->BPMSyncNumerator));
                dump.append("/");
                dump.append(std::to_string(param->BPMSyncDenominator));
            }
            else
            {
                dump.append(std::to_string(plugin->GetParameterValue(i)));
            }
            dump.append("\r\n");
        }
    }
}

std::string PluginProcessor::DumpSettings()
{
    std::string dump;

    if (modeChangeCC != -1)
    {
        dump.append("MapModeController ");
        dump.append(std::to_string(modeChangeCC));
        dump.append("\r\n");
    }

    for (int i = 0; i < 16; i++)
    {
        if (stompCC[i] != -1)
        {
            dump.append("MapStompController ");
            dump.append(std::to_string(i + 1));
            dump.append(" ");
            dump.append(std::to_string(stompCC[i]));
            dump.append("\r\n");
        }
    }

    return dump;
}

std::string PluginProcessor::DumpProgram()
{
    std::string dump;

    dump.append("SetPreset ");
    dump.append(currentPreset);
    dump.append("\r\n");

    for (auto& element : chainList)
    {
        // Only non-"Master" elements get saved in preset
        if (!element->IsMaster)
        {
            if (element->IsChain)
            {
                dump.append("SetChain ");
                dump.append(element->Name);
                dump.append(" ");
            }
            else
            {
                dump.append("SetPluginSlot ");
                dump.append(element->Name);
                dump.append(" ");
            }

            auto& plugins = element->Plugins.GetRead();

            for (auto& plugin : plugins)
            {
                dump.append(plugin->ID);
                dump.append(" ");
            }

            dump.append("\r\n");

            for (const auto& plugin : plugins)
            {
                AppendPluginParams(dump, plugin, false);
            }
        }
    }

    for (auto& entry : ccMapEntries)
    {
        dump.append("MapController ");
        dump.append(std::to_string(entry.CCNumber));
        dump.append(" ");
        dump.append(entry.Plugin->ID);

        if (entry.Parameter.empty())
        {
            dump.append(" Enabled");
        }
        else
        {
            dump.append(" ");
            dump.append(entry.Parameter);
        }

        dump.append("\r\n");
    }

    dump.append("EndProgram\r\n");

    return dump;
}

// This is pretty gross - should probably read presets into a list and keep it around...
std::string PluginProcessor::GetNextPreset(std::string afterPreset)
{
    bool isFirst = true;
    bool returnNext = false;
    std::string first;

    for (const auto& entry : std::filesystem::directory_iterator(presetPath))
    {
        if (returnNext)
        {
            return entry.path().filename().string();
        }

        if (isFirst)
        {
            first = entry.path().filename().string();

            isFirst = false;
        }

        if (entry.path().filename().string() == afterPreset)
        {
            returnNext = true;
        }
    }

    return first;
}

std::string PluginProcessor::GetPreviousPreset(std::string beforePreset)
{
    bool isFirst = true;
    std::string last;

    for (const auto& entry : std::filesystem::directory_iterator(presetPath))
    {
        if (entry.path().filename().string() == beforePreset)
        {
            if (!isFirst)
                return last;
        }

        isFirst = false;

        last = entry.path().filename().string();
    }

    return last;
}


std::string PluginProcessor::HandleCommand(std::string const& line)
{
    std::string err;

    std::cerr << "Got message: " << line << "\n";

    std::vector<std::string> commandWords;
    std::istringstream iss(line);
    std::string s;

    while (iss >> std::quoted(s)) {
        commandWords.push_back(s);
    }

    for (std::string word; iss >> word; )
    {
        commandWords.push_back(word);
    }

    if (commandWords.size() > 0)
    {
        if ((commandWords[0] == "Ok") || (commandWords[0] == "Error"))
        {
            // Handle responses
        }
        else if (commandWords[0] == "Shutdown")
        {
#ifdef __linux__
            system("shutdown -P now");
#endif
        }
        else if (commandWords[0] == "SetGlobalChain")
        {
            // Fixme: not freeing chain elements
            chainList.clear();

            for (size_t cmd = 1; cmd < commandWords.size(); cmd+=2)
            {
                if (cmd == commandWords.size() - 1)
                    break;

                if ((commandWords[cmd] == "Slot") || (commandWords[cmd] == "MasterSlot"))
                {
                    auto iter = chainLookup.find(commandWords[cmd + 1]);

                    if (iter == chainLookup.end())
                    {
                        auto slot = new ChainElement();
                        slot->IsChain = false;
                        slot->Name = commandWords[cmd + 1];
                        slot->IsMaster = (commandWords[cmd] == "MasterSlot");

                        chainLookup[slot->Name] = slot;
                        chainList.push_back(slot);
                    }
                    else
                    {
                        chainList.push_back(iter->second);
                    }
                }
                else if ((commandWords[cmd] == "Chain") || (commandWords[cmd] == "MasterChain") || (commandWords[cmd] == "SplitChain"))
                {
                    auto iter = chainLookup.find(commandWords[cmd + 1]);

                    if (iter == chainLookup.end())
                    {
                        auto chain = new ChainElement();
                        chain->IsChain = true;
                        chain->Name = commandWords[cmd + 1];
                        chain->IsMaster = (commandWords[cmd] == "MasterChain");
                        chain->IsSplit = (commandWords[cmd] == "SplitChain");

                        chainLookup[chain->Name] = chain;
                        chainList.push_back(chain);
                    }
                    else
                    {
                        chainList.push_back(iter->second);
                    }
                }
            }
        }
        else if (commandWords[0] == "SetPluginSlot")
        {
            if (commandWords.size() > 2)
            {
                SetPluginSlot(commandWords[1], commandWords[2]);

                needPluginUpdate = true;
            }
        }
        else if (commandWords[0] == "SetParam")
        {
            if (commandWords.size() > 1)
            {
                StompBox* component = pluginFactory.FindPlugin(commandWords[1]);

                if (component == nullptr)
                {
                    err.append("Bad Plugin");

                    std::cerr << err << "\n";
                }
                else
                {
                    std::cerr << "Set on: " << commandWords[1] << "\n";

                    bool gotParam = false;

                    if (commandWords.size() > 2)
                    {
                        if (commandWords[2] == "Enabled")
                        {
                            if (commandWords.size() > 3)
                            {
                                component->Enabled = (atoi(commandWords[3].c_str()) == 1);

                                CheckMidiCommand(component, nullptr);
                            }

                            gotParam = true;
                        }
                        else
                        {
                            StompBoxParameter* param = component->GetParameter(commandWords[2]);

                            if (param != nullptr)
                            {
                                std::cerr << "Set on parameter: " << commandWords[2] << "\n";

                                if (commandWords.size() > 3)
                                {
                                    if (param->ParameterType == PARAMETER_TYPE_ENUM)
                                    {
                                        std::string paramValue = commandWords[3];

                                        // Concatenate any additional words - should probably enclose in quotes or something instead
                                        for (size_t i = 4; i < commandWords.size(); i++)
                                        {
                                            paramValue += " " + commandWords[i];
                                        }

                                        const std::vector<std::string>* enumValues = param->EnumValues;

                                        bool gotEnum = false;

                                        for (size_t e = 0; e < enumValues->size(); e++)
                                        {
                                            if ((*enumValues)[e] == paramValue)
                                            {
                                                param->SetValue((float)e);

                                                gotEnum = true;

                                                break;
                                            }
                                        }

                                        if (!gotEnum)
                                        {
                                            err.append("Bad Enum Value");

                                            param->SetValue(-1);
                                        }
                                    }
                                    else if (param->ParameterType == PARAMETER_TYPE_FILE)
                                    {
                                        std::string paramValue = commandWords[3];

                                        // Concatenate any additional words - should probably enclose in quotes or something instead
                                        for (size_t i = 4; i < commandWords.size(); i++)
                                        {
                                            paramValue += " " + commandWords[i];
                                        }

                                        const std::vector<std::string>* enumValues = param->EnumValues;

                                        bool gotEnum = false;

                                        for (size_t e = 0; e < enumValues->size(); e++)
                                        {
                                            if ((*enumValues)[e] == paramValue)
                                            {
                                                param->SetValue((float)e);

                                                gotEnum = true;

                                                break;
                                            }
                                        }

                                        if (!gotEnum)
                                        {
                                            err.append("Bad Enum Value");

                                            param->SetValue(-1);
                                        }
                                    }

                                    else if (param->CanSyncToHostBPM && (commandWords[3].find('/') != std::string::npos))
                                    {
                                        size_t pos = commandWords[3].find('/');

                                        param->BPMSyncNumerator = atoi(commandWords[3].substr(0, pos).c_str());
                                        param->BPMSyncDenominator = atoi(commandWords[3].c_str() + pos + 1);

                                        component->UpdateBPM();
                                    }
                                    else
                                    {
                                        if (param->CanSyncToHostBPM)
                                        {
                                            param->BPMSyncNumerator = param->BPMSyncDenominator = 0;
                                        }

                                        param->SetValue((float)atof(commandWords[3].c_str()));

                                        CheckMidiCommand(component, param);
                                    }

                                    gotParam = true;
                                }
                                else
                                {
                                    // No parameter value, so set default value

                                    std::cout << "Set default value" << std::endl;

                                    param->SetValue(param->DefaultValue);
                                }
                            }
                        }
                    }

                    if (!gotParam)
                    {
                        err.append("Bad Parameter");
                    }
                }
            }
        }
        else if (commandWords[0] == "SetChain")
        {
            if (commandWords.size() > 1)
            {
                auto chain = GetChain(commandWords[1]);

                if (chain != nullptr)
                {
                    size_t numPlugins = commandWords.size() - 2;

                    std::cerr << "SetChain " << commandWords[1] << "\n";

                    auto& plugins = chain->Plugins.GetWriteLock();

                    plugins.clear();

                    for (size_t i = 0; i < numPlugins; i++)
                    {
                        std::cerr << commandWords[i + 2] << "\n";

                        StompBox* newComponent = CreatePlugin(commandWords[i + 2]);

                        if (newComponent != nullptr)
                            plugins.push_back(newComponent);
                    }

                    chain->Plugins.FinishWrite();

                    needPluginUpdate = true;
                }
                else
                {
                    err = "Bad Chain";
                }
            }
        }
        else if (commandWords[0] == "ReleasePlugin")
        {
            if (commandWords.size() > 1)
            {
                pluginFactory.ReleasePlugin(commandWords[1]);
            }
        }
        else if (commandWords[0] == "Dump")
        {
            if (commandWords.size() > 1)
            {
                if (commandWords[1] == "Program")
                {
                    SendClientMessage(DumpSettings());
                    SendClientMessage(DumpProgram());
                }
                else if (commandWords[1] == "Config")
                {
                    SendClientMessage(DumpConfig());
                }
                else if (commandWords[1] == "Version")
                {
                    SendClientMessage("Version " + GetVersion());
                }
            }
        }
        else if (commandWords[0] == "List")
        {
            if (commandWords.size() > 1)
            {
                if (commandWords[1] == "Presets")
                {
                    std::string presetList = "Presets ";

                    for (const auto& preset : GetPresets())
                    {
                        presetList.append(preset);
                        presetList.append(" ");
                    }

                    presetList.append("\r\n");

                    SendClientMessage(presetList);
                }
            }
        }
        else if (commandWords[0] == "SavePreset")
        {
            if (commandWords.size() > 1)
            {
                currentPreset = commandWords[1];

                std::string dump = DumpProgram();

                std::filesystem::path outPath;
                
                outPath.assign(presetPath);
                outPath.append(currentPreset);

                std::ofstream outFile(outPath, std::ios::binary);

                outFile << dump;

                outFile.close();

                ScanPresets();
            }
        }
        else if (commandWords[0] == "LoadPreset")
        {
            if (commandWords.size() > 1)
            {
                LoadPreset(commandWords[1]);
            }
        }
        else if (commandWords[0] == "DeletePreset")
        {
            if (commandWords.size() > 1)
            {
                std::filesystem::path outPath;

                outPath.assign(presetPath);
                outPath.append(commandWords[1]);

                if (std::filesystem::exists(outPath))
                    std::filesystem::remove(outPath);

                ScanPresets();
            }
        }
        else if (commandWords[0] == "SaveSettings")
        {
            SaveSettings();
        }
        else if (commandWords[0] == "MapController")
        {
            if (commandWords.size() > 2)
            {
                CCMap ccMap;

                ccMap.CCNumber = atoi(commandWords[1].c_str());

                if (commandWords.size() > 3)
                {
                    StompBox* plugin = pluginFactory.FindPlugin(commandWords[2]);

                    ccMap.Plugin = plugin;

                    if (plugin != nullptr)
                    {
                        if (commandWords[3] == "Enabled")
                        {
                            ccMap.Parameter = "";

                            ccMapEntries.push_back(ccMap);
                        }
                        else
                        {
                            ccMap.Parameter.assign(commandWords[3]);

                            ccMapEntries.push_back(ccMap);
                        }
                    }
                }
            }
        }
        else if (commandWords[0] == "ClearControllerMap")
        {
            ccMapEntries.clear();
        }
        else if (commandWords[0] == "MapStompController")
        {
            if (commandWords.size() > 2)
            {
                int stompIndex = atoi(commandWords[1].c_str()) - 1;
                int ccNumber = atoi(commandWords[2].c_str());

                stompCC[stompIndex] = ccNumber;
            }
        }
        else if (commandWords[0] == "MapModeController")
        {
            if (commandWords.size() > 1)
            {
                modeChangeCC = atoi(commandWords[1].c_str());
            }
        }
        else if (commandWords[0] == "SendCommand")
        {
            if (commandWords.size() > 1)
            {
                StompBox* component = pluginFactory.FindPlugin(commandWords[1]);

                if (component == nullptr)
                {
                    err.append("Bad Plugin");

                    std::cerr << err << "\n";
                }
                else
                {
                    component->HandleCommand(commandWords);
                }
            }
        }
        else if (commandWords[0] == "PluginOutputOn")
        {
            pluginOutputOn = true;
        }
        else if (commandWords[0] == "PluginOutputOff")
        {
            pluginOutputOn = false;
        }
        else if (commandWords[0] == "HandleMidi")
        {
            if (commandWords.size() > 3)
            {
                int midiCommand = atoi(commandWords[1].c_str());
                int midiData1 = atoi(commandWords[2].c_str());
                int midiData2 = atoi(commandWords[3].c_str());

                HandleMidiCommand(midiCommand, midiData1, midiData2);
            }
        }
        else if (commandWords[0] == "MidiLockOn")
        {
            midiLocked = true;
        }
        else if (commandWords[0] == "MidiLockOff")
        {
            midiLocked = false;
        }
        else
        {
            err.append("Bad Command");
        }
    }

    return err;
}

bool PluginProcessor::HandleMidiCommand(int midiCommand, int midiData1, int midiData2)
{
    if (midiLocked)
        return false;

    if (midiCommand == 0xB0) // Control change
    {        
        bool handled = false;

        if (midiData1 == modeChangeCC)
        {
            switch (currentMidiMode)
            {
                case MIDI_MODE_PRESETCHANGE:
                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.SetPresetText(currentPreset.c_str(), PRESET_TEXT_COLOR);
                    }
                    break;

                case MIDI_MODE_TUNER:
                    if (tuner != nullptr)
                    {
                        tuner->Enabled = false;
                        tuner->EnabledIsDirty = true;
                    }
                    break;
            }

            currentMidiMode++;

            if (currentMidiMode == MIDI_MODE_MAX)
                currentMidiMode = 0;

            switch (currentMidiMode)
            {
                case MIDI_MODE_STOMP:
                    SyncPreset();
                    break;

                case MIDI_MODE_PRESETCHANGE:
                    previewPreset = currentPreset;

                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.SetPresetText(previewPreset.c_str(), PRESET_PREVIEW_COLOR);
                    }
                    break;

                case MIDI_MODE_TUNER:
                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.ResetDisplay();
                        serialDisplayInterface.SetPresetText("Tuner", PRESET_TEXT_COLOR);
                        serialDisplayInterface.ResetTuner();
                    }

                    if (tuner != nullptr)
                    {
                        tuner->Enabled = true;
                        tuner->EnabledIsDirty = true;
                    }
                    else
                    {
                        fprintf(stderr, "No tuner plugin available\n");
                    }

                    break;

                case MIDI_MODE_RECORDER:
                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.ResetDisplay();
                        serialDisplayInterface.SetPresetText("Recorder", PRESET_TEXT_COLOR);
                        serialDisplayInterface.ResetTuner();
                    }

                    break;
            }

            handled = true;
        }
        else
        {
            switch (currentMidiMode)
            {
            case MIDI_MODE_STOMP:
                for (auto& ccMap : ccMapEntries)
                {
                    if (ccMap.CCNumber == midiData1)
                    {
                        fprintf(stderr, "Got CC %d\n", midiData1);

                        fprintf(stderr, "Plugin is %s\n", ccMap.Plugin->Name.c_str());

                        if (ccMap.Parameter.empty()) // Enabled
                        {
                            ccMap.Plugin->Enabled = !ccMap.Plugin->Enabled; // (midiData2 > 1);

                            CheckMidiCommand(ccMap.Plugin, nullptr);

                            ccMap.Plugin->EnabledIsDirty = true;
                        }
                        else
                        {
                            StompBoxParameter* parameter = ccMap.Plugin->GetParameter(ccMap.Parameter);

                            if (parameter != nullptr)
                            {
                                float value = parameter->MinValue + ((parameter->MaxValue - parameter->MinValue) * ((float)midiData2 / 127.0f));

                                parameter->SetValue(value);

                                CheckMidiCommand(ccMap.Plugin, parameter);

                                ccMap.Plugin->ParamIsDirty = true;
                                parameter->IsDirty = true;
                            }
                        }

                        handled = true;
                    }
                }
                break;

            case MIDI_MODE_PRESETCHANGE:
                if ((stompCC[0] != -1) && (midiData1 == stompCC[0]))
                {
                    previewPreset.assign(GetPreviousPreset(previewPreset));
                    serialDisplayInterface.SetPresetText(previewPreset.c_str(), PRESET_PREVIEW_COLOR);
                }
                else if ((stompCC[1] != -1) && (midiData1 == stompCC[1]))
                {
                    previewPreset.assign(GetNextPreset(previewPreset));
                    serialDisplayInterface.SetPresetText(previewPreset.c_str(), PRESET_PREVIEW_COLOR);
                }
                else if ((stompCC[2] != -1) && (midiData1 == stompCC[2]))
                {
                    LoadPreset(previewPreset);
                }
                break;

            case MIDI_MODE_RECORDER:
                if ((stompCC[2] != -1) && (midiData1 == stompCC[2]))
                {
                    if ((audioFileRecorder != nullptr) && audioFileRecorder->Enabled)
                    {
                        std::thread saveThread = std::thread(&AudioFileRecorder::SaveRecording, (AudioFileRecorder *)audioFileRecorder);
                        saveThread.detach();
                    }
                }
                break;
            }
        }

        return handled;
    }
    else if (midiCommand == 0xC0) // Patch change
    {
        fprintf(stderr, "MIDI program change to %d\n", midiData1);

        for (const auto& entry : std::filesystem::directory_iterator(presetPath))
        {
            std::string preset = entry.path().filename().string();

            if (atoi(preset.substr(0, 2).c_str()) == (midiData1 + 1))
            {
                LoadPreset(preset);

                break;
            }
        }
    }

    return false;
}

bool PluginProcessor::CheckMidiCommand(StompBox* plugin, StompBoxParameter* parameter)
{
    if (serialDisplayInterface.IsConnected())
    {
        for (auto& ccMap : ccMapEntries)
        {
            if ((ccMap.Plugin == plugin) && ((ccMap.Parameter.empty() && (parameter == nullptr)) || ((parameter != nullptr) && (ccMap.Parameter == parameter->Name))))
            {
                int16_t stomp = -1;

                for (int16_t i = 0; i < 16; i++)
                {
                    if (stompCC[i] == ccMap.CCNumber)
                    {
                        stomp = i;

                        break;
                    }
                }

                if (stomp != -1)
                {
                    if (ccMap.Parameter.empty())
                    {
                        serialDisplayInterface.HandleStomp(stomp, ccMap.Plugin->Enabled, ccMap.Plugin->Name.c_str(), "Enabled",
                            ccMap.Plugin->BackgroundColor.c_str(), ccMap.Plugin->ForegroundColor.c_str());
                    }
                    else
                    {
                        serialDisplayInterface.HandleStomp(stomp, (parameter->GetValue()) > 0, ccMap.Plugin->Name.c_str(), parameter->Name.c_str(),
                            ccMap.Plugin->BackgroundColor.c_str(), ccMap.Plugin->ForegroundColor.c_str());
                    }
                }
            }
        }
    }

    return false;
}

void PluginProcessor::SyncPreset()
{
    serialDisplayInterface.ResetDisplay();
    serialDisplayInterface.SetPresetText(currentPreset.c_str(), PRESET_TEXT_COLOR);

    for (auto& ccMap : ccMapEntries)
    {
        CheckMidiCommand(ccMap.Plugin, ccMap.Plugin->GetParameter(ccMap.Parameter));
    }
}

void PluginProcessor::LoadPreset(std::string preset)
{
    std::cout << "Loading preset: " << preset << std::endl;

    if (ramp == 0)
    {
        loadPreset.assign(preset);

        StartRamp(-1, defaultRampMS);
    }
    else
    {
        std::cout << "Error: Can't load preset while another is still loading" << std::endl;
    }
}

void PluginProcessor::ThreadLoadPreset()
{
    std::filesystem::path inPath;

    inPath.assign(presetPath);
    inPath.append(loadPreset);

    // Clear any MIDI CC mappings we have
    ccMapEntries.clear();

    if (LoadCommandsFromFile(inPath))
    {
        currentPreset.assign(loadPreset);

        if (serialDisplayInterface.IsConnected())
        {
            SyncPreset();
        }

        currentMidiMode = MIDI_MODE_STOMP;
    }

    UpdatePlugins();

    StartRamp(1, defaultRampMS);

    if (stompboxServer.HaveClient())
    {
        std::string dump = DumpProgram();

        SendClientMessage(dump);
    }
}

void PluginProcessor::LoadSettings()
{
    std::filesystem::path settingsPath;

    settingsPath.assign(dataPath);
    settingsPath.append("StompboxSettings");

    LoadCommandsFromFile(settingsPath);
}

void PluginProcessor::SaveSettings()
{
    std::string dump = DumpSettings();

    std::filesystem::path settingsPath;

    settingsPath.assign(dataPath);
    settingsPath.append("StompboxSettings");

    std::ofstream outFile(settingsPath, std::ios::binary);

    outFile << dump;

    outFile.close();
}

bool PluginProcessor::LoadCommandsFromFile(std::filesystem::path filePath)
{
    std::cout << "Load commands from file :" << filePath << std::endl;

    if (!std::filesystem::exists(filePath))
        return false;

    std::ifstream inFile(filePath);

    std::string line;

    while (std::getline(inFile, line))
    {
        if (line[line.size() - 1] == '\r')
        {
            HandleCommand(line.substr(0, line.size() - 1));
        }
        else
        {
            HandleCommand(line);
        }
    }

    return true;
}

void PluginProcessor::StartRamp(int rampDirection, float rampMS)
{
    rampSamplesSoFar = 0;
    ramp = rampDirection;
    rampSamples = (int)(sampleRate * (rampMS / 1000.0f));
}

void PluginProcessor::Process(float* input, float* output, size_t count)
{
    if (ramp != 0)
    {
        if (rampSamplesSoFar == rampSamples)
        {
            if (ramp == -1)
            {
                if (presetLoadThread == nullptr)
                {
                    presetLoadThread = new std::thread(&PluginProcessor::ThreadLoadPreset, this);
                }
            }
            else
            {
                presetLoadThread->join();
                presetLoadThread = nullptr;

                ramp = 0;
            }
        }
    }
    else
    {
        if (needPluginUpdate)
            UpdatePlugins();
    }

    // Disable floating point denormals
    std::fenv_t fe_state;
    std::feholdexcept(&fe_state);
    disable_denormals();

    memcpy(output, input, count * sizeof(float));

    if ((ramp == 0) || (rampSamplesSoFar != rampSamples))
    {
        bool inSplit = false;

        for (auto& element : chainList)
        {
            if (element->IsSplit)
            {
                memcpy(splitBuffer.data(), output, count * sizeof(float));
            }

            for (auto& plugin : element->Plugins.GetRead())
            {
                float* processBuffer = inSplit ? splitBuffer.data() : output;

                if (plugin->Enabled)
                {
                    if (plugin->InputGain != nullptr)
                        plugin->InputGain->compute((int)count, processBuffer, processBuffer);

                    plugin->compute((int)count, processBuffer, processBuffer);

                    if (plugin->OutputVolume != nullptr)
                        plugin->OutputVolume->compute((int)count, processBuffer, processBuffer);
                }

                if (plugin == monitorPlugin)
                {
                    monitorCallback(processBuffer, (int)count);
                }
            }

            if (inSplit)
            {
                // Merge our split data back
                for (int i = 0; i < count; i++)
                {
                    output[i] += splitBuffer[i];
                }
            }

            inSplit = element->IsSplit;
        }
    }

    if (ramp != 0)
    {
        for (size_t i = 0; i < count; i++)
        {
            float rampPercent = (float)rampSamplesSoFar / (float)rampSamples;

            rampSamplesSoFar = std::min(rampSamplesSoFar + 1, rampSamples);

            output[i] *= (ramp == -1) ? (1.0f - rampPercent) : rampPercent;
        }
    }

    // restore previous floating point state
    std::feupdateenv(&fe_state);
}
