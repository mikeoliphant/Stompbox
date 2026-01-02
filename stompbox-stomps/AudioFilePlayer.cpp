#include "AudioFilePlayer.h"

AudioFilePlayer::AudioFilePlayer(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
	fileType(folderName, fileExtensions, basePath)
{
	Name = "AudioFilePlayer";

	auto& fileParam = AddParameter();
	fileParam.Name = "File";
	fileParam.SourceVariable = &fileIndex;
	fileParam.ParameterType = PARAMETER_TYPE_FILE;
	fileParam.DefaultValue = -1;
	fileParam.FilePath = fileType.GetFolderName();
	fileParam.EnumValues = &fileType.GetFileNames();
	fileParam.MinValue = -1;
	fileParam.MaxValue = (float)(fileType.GetFileNames().size()) - 1;

	auto& levelParam = AddParameter();
	levelParam.Name = "Level";
	levelParam.SourceVariable = &level;
	levelParam.MaxValue = 1.5;
	levelParam.DefaultValue = level;

	auto& playingParam = AddParameter();
	playingParam.Name = "Playing";
	playingParam.ParameterType = PARAMETER_TYPE_BOOL;
	playingParam.SourceVariable = &playing;
	playingParam.DefaultValue = playing;
	playingParam.SuppressSave = true;

	auto& positionParam = AddParameter();
	positionParam.Name = "Position";
	positionParam.SourceVariable = &position;
	positionParam.DefaultValue = position;
	positionParam.SuppressSave = true;
}

void AudioFilePlayer::SetParameterValue(StompBoxParameter& param, float value)
{
	StompBox::SetParameterValue(param, value);

	if (param.Name == "File")
	{
		needWaveLoad = true;
	}
	else if (param.Name == "Position")
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