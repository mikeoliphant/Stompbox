#pragma once

#include <vector>
#include <lv2/core/lv2.h>
#include <lv2/urid/urid.h>
#include <lv2/worker/worker.h>
#include "PluginProcessor.h"

class StompboxLV2PluginFactory;
extern StompboxLV2PluginFactory* factory;
extern void connect_port(LV2_Handle instance, uint32_t port, void* data);
extern void activate(LV2_Handle);
extern void run(LV2_Handle instance, uint32_t n_samples);
extern void deactivate(LV2_Handle);
extern void cleanup(LV2_Handle instance);
extern const void* extension_data(const char* uri);

template<typename T>
static LV2_Handle instantiate(const LV2_Descriptor*, double rate, const char*, const LV2_Feature* const* features)
{
	auto plugin = new T();

	plugin->Init(rate);

	return static_cast<LV2_Handle>(plugin);
}

class StompboxLV2Port
{
	public:
		StompboxLV2Port() :
			dataPtr(nullptr)
		{
		}
		virtual void Update() {};
		void SetDataPtr(void *ptr)
		{
			this->dataPtr = ptr;
		}

	protected:
		void *dataPtr;
};

class StompboxLV2AtomPort : public StompboxLV2Port
{	
};

class StompboxLV2AudioPort : public StompboxLV2Port
{
	public:
		float* GetAudioData()
		{
			return static_cast<float *>(dataPtr);
		}
};

class StompboxLV2ControlPort : public StompboxLV2Port
{
	public:
		StompboxLV2ControlPort() :
			parameter(nullptr)
		{			
		}

		StompboxLV2ControlPort(StompBoxParameter* param)
			: StompboxLV2ControlPort()
		{
			SetParameter(param);
		}

		void SetParameter(StompBoxParameter* param)
		{
			this->parameter = param;
		}

		void Update() override
		{
			if (dataPtr != nullptr)
			{
				parameter->SetValue(*(static_cast<float*>(dataPtr)));
			}
		}

	private:
		StompBoxParameter* parameter;
};

class StompboxLV2Plugin
{
public:
	StompboxLV2Plugin() :
		controlAtomPort(),
		inputAudioPort(),
		outputAudioPort(),
		processor("./", false)
	{
		//AddPort(controlAtomPort);
		AddPort(inputAudioPort);
		AddPort(outputAudioPort);
	}

	virtual ~StompboxLV2Plugin() {};

	void Init(float sampleRate)
	{
		processor.Init(sampleRate);
	}

	void AddPort(StompboxLV2Port& port)
	{
		ports.push_back(&port);
	}

	void SetPortDataPtr(uint32_t portIndex, void *dataPtr)
	{
		if (portIndex < ports.size())
		{
			ports[portIndex]->SetDataPtr(dataPtr);
		}
	}

	virtual void Process(uint32_t numSamples)
	{
		for (auto port : ports)
		{
			port->Update();
		}

		auto inData = inputAudioPort.GetAudioData();
		auto outData = outputAudioPort.GetAudioData();

		processor.Process(inData, outData, numSamples);
	}

	protected:
		PluginProcessor processor;
		StompboxLV2AtomPort controlAtomPort;
		StompboxLV2AudioPort inputAudioPort;
		StompboxLV2AudioPort outputAudioPort;
		std::vector<StompboxLV2Port*> ports;

	private:
};

class StompboxLV2PluginFactory
{
	public:
		LV2_Descriptor* GetDescriptor(uint32_t index)
		{
			if (index < descriptors.size())
			{
				return &descriptors[index];
			}

			return nullptr;
		}

	protected:
		template<typename T>
		void AddPlugin(const char* URI)
		{
			descriptors.emplace_back(LV2_Descriptor {
				URI,
				instantiate<T>,
				connect_port,
				activate,
				run,
				deactivate,
				cleanup,
				extension_data
				});
		}

	private:
		std::vector<LV2_Descriptor> descriptors;
};
