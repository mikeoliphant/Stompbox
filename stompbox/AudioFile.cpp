#define DR_WAV_IMPLEMENTATION

#include <iostream>
#include "AudioFile.h"
#include "CDSPResampler.h"

WaveReader::WaveReader(const std::string filename, const uint32_t desiredSampleRate)
{
	memset(&waveHeader, 0, sizeof(waveHeader));

	unsigned int channels;
	unsigned int sampleRate;
	drwav_uint64 numFrames;

	waveData = drwav_open_file_and_read_pcm_frames_f32(filename.c_str(), &channels, &sampleRate, &numFrames, nullptr);

	if (waveData != nullptr)
	{
		NumSamples = (size_t)numFrames;
		NumChannels = (size_t)channels;
		SampleRate = (uint32_t)sampleRate;

		if (SampleRate != desiredSampleRate)
		{
			r8b::CDSPResampler16IR resampler(sampleRate, desiredSampleRate, (int)numFrames);
			int resampledFrames = resampler.getMaxOutLen(0);

			float *resampled = new float[resampledFrames];
			resampler.oneshot(waveData, (int)numFrames, resampled, resampledFrames);

			delete[] waveData;

			waveData = resampled;

			NumSamples = resampledFrames;
			SampleRate = desiredSampleRate;

			//std::ofstream waveFile;

			//waveFile.open("c:\\tmp\\resample.wav", std::ios::trunc | std::ios::binary);

			//waveFile.write((char*)waveData, NumSamples * 4);

			//waveFile.close();
		}
	}
};

WaveReader::~WaveReader()
{
	if (waveData != nullptr)
	{
		delete[] waveData;
	}
};


WaveWriter::WaveWriter(int sampleRate, size_t recordSeconds)
{
	this->sampleRate = sampleRate;
	this->recordSeconds = recordSeconds;

	waveDataSize = (size_t)(recordSeconds * sampleRate * 3);

	waveData = new char[waveDataSize];

	ResetRecording();
};

WaveWriter::~WaveWriter()
{
	delete[] waveData;
}

float WaveWriter::GetRecordSeconds()
{
	if (recordIsFull)
		return (float)recordSeconds;

	return ((float)recordPos / (float)waveDataSize) * recordSeconds;
}

void WaveWriter::ResetRecording()
{
	needReset = true;
}

void WaveWriter::AddSamples(float* samples, size_t numSamples)
{
	if (needReset)
	{
		recordPos = 0;
		recordIsFull = false;

		needReset = false;
	}

	size_t pos = recordPos;	// Make a copy in case recordPos gets changed while we're writing

	for (size_t i = 0; i < numSamples; i++)
	{
		int32_t intSample = (int32_t)(samples[i] * INT32_MAX);

		waveData[pos++] = ((intSample >> 8) & 0xff);
		waveData[pos++] = ((intSample >> 16) & 0xff);
		waveData[pos++] = ((intSample >> 24) & 0xff);

		if (pos == waveDataSize)
		{
			recordIsFull = true;

			pos = 0;
		}
	}

	recordPos = pos;
};

void WaveWriter::WriteToFile(std::string filename)
{
	WaveHeader waveHeader;

	size_t recordDataSize;

	if (recordIsFull)
		recordDataSize = waveDataSize;
	else
	{
		recordDataSize = recordPos;
	}

	std::cout << "AudioFileRecorder saving " << recordDataSize << " bytes" << std::endl;

	memcpy((char*)waveHeader.fmt, "fmt ", 4);
	waveHeader.Subchunk1Size = 16;
	waveHeader.AudioFormat = 1;
	waveHeader.NumOfChan = 1;
	waveHeader.SamplesPerSec = sampleRate;
	waveHeader.blockAlign = 3;
	waveHeader.bytesPerSec = sampleRate * waveHeader.blockAlign;
	waveHeader.bitsPerSample = 3 * 8;


	memcpy((char*)waveHeader.RIFF, "RIFF", 4);
	memcpy((char*)waveHeader.WAVE, "WAVE", 4);

	waveHeader.ChunkSize = (uint32_t)(36 + recordDataSize);

	std::ofstream waveFile;

	waveFile.open(filename, std::ios::trunc | std::ios::binary);

	waveFile.write((char*)&waveHeader, sizeof(waveHeader));

	waveFile.write("data", 4);
	waveFile.write((char*)&recordDataSize, 4);

	if (recordIsFull)
	{
		waveFile.write(waveData + recordPos, (std::streamsize)waveDataSize - recordPos);
	}

	waveFile.write(waveData, recordPos);

	waveFile.close();

	std::cout << "AudioFileRecorder save finished" << std::endl;
}
