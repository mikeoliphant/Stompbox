#include "AudioFilePlayer.h"

AudioFilePlayer::AudioFilePlayer(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
	fileType(folderName, fileExtensions, basePath)
{
	Name = "AudioFilePlayer";

	NumParameters = AUDIOFILEPLAYER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[AUDIOFILEPLAYER_FILE].Name = "File";
	Parameters[AUDIOFILEPLAYER_FILE].SourceVariable = &fileIndex;
	Parameters[AUDIOFILEPLAYER_FILE].ParameterType = PARAMETER_TYPE_FILE;
	Parameters[AUDIOFILEPLAYER_FILE].DefaultValue = -1;
	Parameters[AUDIOFILEPLAYER_FILE].FilePath = fileType.GetFolderName();
	Parameters[AUDIOFILEPLAYER_FILE].EnumValues = &fileType.GetFileNames();
	Parameters[AUDIOFILEPLAYER_FILE].MinValue = -1;
	Parameters[AUDIOFILEPLAYER_FILE].MaxValue = (float)(fileType.GetFileNames().size()) - 1;

	Parameters[AUDIOFILEPLAYER_LEVEL].Name = "Level";
	Parameters[AUDIOFILEPLAYER_LEVEL].SourceVariable = &level;
	Parameters[AUDIOFILEPLAYER_LEVEL].MaxValue = 1.5;
	Parameters[AUDIOFILEPLAYER_LEVEL].DefaultValue = level;

	Parameters[AUDIOFILEPLAYER_PLAYING].Name = "Playing";
	Parameters[AUDIOFILEPLAYER_PLAYING].ParameterType = PARAMETER_TYPE_BOOL;
	Parameters[AUDIOFILEPLAYER_PLAYING].SourceVariable = &playing;
	Parameters[AUDIOFILEPLAYER_PLAYING].DefaultValue = playing;
	Parameters[AUDIOFILEPLAYER_PLAYING].SuppressSave = true;

	Parameters[AUDIOFILEPLAYER_POSITION].Name = "Position";
	Parameters[AUDIOFILEPLAYER_POSITION].SourceVariable = &position;
	Parameters[AUDIOFILEPLAYER_POSITION].DefaultValue = position;
	Parameters[AUDIOFILEPLAYER_POSITION].SuppressSave = true;
}

void AudioFilePlayer::SetParameterValue(StompBoxParameter* param, float value)
{
	StompBox::SetParameterValue(param, value);

	if (param == &Parameters[AUDIOFILEPLAYER_FILE])
	{
		needWaveLoad = true;
	}
	else if (param == &Parameters[AUDIOFILEPLAYER_POSITION])
	{
		if (waveReader)
		{
			readPosition = (int)(waveReader->NumSamples * position);
		}
	}
}

void AudioFilePlayer::HandleCommand(const std::vector<std::string>& commandWords)
{
	if (commandWords.size() > 2)
	{
		if (commandWords[2] == "ArmRecord")
		{
			if (playing == 1)
			{
				recordArmed = true;
				haveRecording = false;
				recording = false;

				fprintf(stderr, "AudioFilePlayer record armed\n");
			}
		}
	}
}

void AudioFilePlayer::SetFile()
{
	recordArmed = false;
	recording = false;
	haveRecording = false;
	readPosition = 0;

	if (waveReader != nullptr)
	{
		delete waveReader;
		waveReader = nullptr;

		delete recordBuffer;
		recordBuffer = nullptr;
	}

	if ((fileIndex >= 0) && (fileType.GetFileNames().size() >= (size_t)fileIndex))
	{
		waveReader = new WaveReader(fileType.GetFilePaths()[(int)fileIndex].string(), (uint32_t)samplingFreq);

		if (waveReader->NumSamples == 0)
		{
			delete waveReader;
			waveReader = nullptr;
		}
		else
		{
			waveBuffer = waveReader->GetWaveData();
			readPosition = 0;

			recordBuffer = new float[waveReader->NumSamples];
		}
	}
}

void AudioFilePlayer::init(int newSamplingFreq)
{
	StompBox::init(newSamplingFreq);
}

void AudioFilePlayer::compute(int count, float* input, float* output)
{
	if (needWaveLoad)
	{
		SetFile();

		needWaveLoad = false;
	}

	if ((playing == 1) && (waveReader != nullptr))
	{
		float linearLevel = (float)(pow(10, level) - 1) / 9;

		size_t leftToRead = count;
		size_t outputPos = 0;

		float* recordInput = input;

		while (leftToRead > 0)
		{			
			size_t toRead = std::min(leftToRead, waveReader->NumSamples - readPosition);

			if (recording)
			{
				memcpy(recordBuffer + readPosition, recordInput, toRead * sizeof(float));

				recordInput += toRead;
			}

			float* readPtr = waveBuffer + (readPosition * waveReader->NumChannels);

			for (size_t i = 0; i < toRead; i++)
			{
				float outputVal = 0;

				for (size_t channel = 0; channel < waveReader->NumChannels; channel++)
				{
					outputVal += *readPtr;

					readPtr++;
				}

				output[outputPos] = input[outputPos] + (outputVal * linearLevel);

				if (haveRecording)
				{
					output[outputPos] += recordBuffer[readPosition + i];
				}

				outputPos++;
			}

			leftToRead -= toRead;
			readPosition += toRead;

			if (leftToRead > 0)
			{
				// Wrap back to beginning of file

				readPosition = 0;

				if (recording)
				{
					// If we are recording, stop

					recordArmed = recording = false;
					haveRecording = true;

					fprintf(stderr, "AudioFilePlayer record finished\n");
				}
				else
				{
					// If recording is armed, turn it on when the file loops around
					if (recordArmed)
					{
						recording = true;

						fprintf(stderr, "AudioFilePlayer record started\n");
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			output[i] = input[i];
		}
	}
}