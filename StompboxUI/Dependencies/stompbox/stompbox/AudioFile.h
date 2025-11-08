#pragma once

#include <limits.h>
#include <string.h>
#include <string>
#include <fstream>
#include <stdint.h>
#include "dr_wav.h"

typedef struct WAVE_HEADER
{
	/* RIFF Chunk Descriptor */
	uint8_t         RIFF[4];        // RIFF Header Magic header
	uint32_t        ChunkSize;      // RIFF Chunk Size
	uint8_t         WAVE[4];        // WAVE Header
	/* "fmt" sub-chunk */
	uint8_t         fmt[4];         // FMT header
	uint32_t        Subchunk1Size;  // Size of the fmt chunk
	uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
	uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
	uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
	uint32_t        bytesPerSec;    // bytes per second
	uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
	uint16_t        bitsPerSample;  // Number of bits per sample
} WaveHeader;

class WaveReader
{
public:
	WaveReader(const std::string filename, uint32_t desiredSampleRate);
	~WaveReader();

	float* GetWaveData()
	{
		return waveData;
	}

	size_t NumChannels = 0;
	uint32_t SampleRate = 0;
	size_t NumSamples = 0;

private:
	WaveHeader waveHeader;
	float* waveData = nullptr;
};

class WaveWriter
{
public:
	WaveWriter(int sampleRate, size_t recordSeconds);
	~WaveWriter();
	float GetRecordSeconds();
	void ResetRecording();
	void AddSamples(float* samples, size_t numSamples);
	void WriteToFile(std::string filename);

private:
	int sampleRate;
	size_t recordSeconds;
	char* waveData = nullptr;
	size_t waveDataSize;
	size_t recordPos;
	bool recordIsFull = false;
	bool needReset = false;
};