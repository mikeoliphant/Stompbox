#pragma once

#include <filesystem>

#ifndef NOMINMAX
#define NOMINMAX  // prevents windows.h from defining min/max macros and messing with juce
#endif

#if _WIN32
#include "windows.h"
#endif

#include <math.h>
#include <algorithm>
#include <memory.h>
#include <sys/stat.h>
#include <fstream>
#include <limits.h>

#include "convoengine.h"

#include "StompBox.h"
#include "AudioFile.h"
#include "FileType.h"

enum
{
	CONVOLVER_IMPULSE,
	CONVOLVER_DRY,
	CONVOLVER_WET,
	CONVOLVER_NUMPARAMETERS
};

enum
{
	IMPUlSE_PARAM_SAMPLERATE
};

struct Convolution
{
	WDL_ImpulseBuffer ImpulseBuffer;
	//WDL_ConvolutionEngine_Div ConvolutionEngine;
	WDL_ConvolutionEngine_Thread ConvolutionEngine;
};

class IRLoader : public FileLoader<Convolution>
{
private:
	float epsilon = 0.0005; // -66dB RMS
	int maxIRSamples = 2048; // FIXME: should be based on sample rate
	int sampleRate = 48000;

protected:
	Convolution* LoadFromFile(const std::filesystem::path& loadPath)
	{
		std::cout << "Loading IR from: " << loadPath << std::endl;
			
		WaveReader* waveReader = new WaveReader(loadPath.string(), (int)sampleRate);

		float* waveData = waveReader->GetWaveData();

		if (waveData != nullptr)
		{
			size_t endPos = waveReader->NumSamples;

			if ((maxIRSamples > 0) && (endPos > (size_t)maxIRSamples))
				endPos = (size_t)maxIRSamples;

			endPos--;

			size_t numGreater = 0;

			while (endPos >= 0)
			{
				if (abs(waveData[endPos]) > epsilon)
				{
					numGreater++;

					if (numGreater > 10)
					{
						endPos += 10;

						break;
					}
				}
				else
				{
					numGreater = 0;
				}

				endPos--;
			}

			size_t numSamples = endPos + 1;

			std::cout << "IR size: " << waveReader->NumSamples << " trimmed to: " << numSamples << std::endl;

			float* data = new float[numSamples];

			float gain = (float)pow(10, -18 * 0.05);  // IRs are usually too loud

			for (size_t i = 0; i < numSamples; i++)
			{
				data[i] = waveData[i] * gain;
			}

			Convolution* conv = new Convolution();

			float* newImpulseData[1];

			newImpulseData[0] = data;

			conv->ImpulseBuffer.Set((const float**)newImpulseData, (int)numSamples, 1);
			conv->ConvolutionEngine.SetImpulse(&conv->ImpulseBuffer);

			delete waveReader;
			delete[] data;

			return conv;
		}

		std::cout << "** IR load failed" << std::endl;

		return nullptr;
	}

public:
	IRLoader(const FileType& fileType) :
		FileLoader<Convolution>(fileType)
	{
	}

	void SetEpsilon(float newEpsilon)
	{
		epsilon = newEpsilon;
	}

	void SetMaxIRSamples(int maxSamples)
	{
		maxIRSamples = maxSamples;
	}

	void SetSampleRate(int newSampleRate)
	{
		sampleRate = newSampleRate;
	}
};


class GuitarConvolver : public StompBox
{
private:
	float impulseIndex = -1;
	float wet = 1;
	float dry = 0;
	FileType fileType;
	IRLoader irLoader;

public:
	GuitarConvolver(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath);
	virtual ~GuitarConvolver() {}
	virtual void init(int samplingFreq);
	void SetParameterValue(StompBoxParameter* parameter, float value);
	void SetMaxIRSamples(int numSamples)
	{
		irLoader.SetMaxIRSamples(numSamples);
	}
	void SetIREpsilon(float epsilon)
	{
		irLoader.SetEpsilon(epsilon);
	}
	virtual void compute(int count, float* input, float* output);
};