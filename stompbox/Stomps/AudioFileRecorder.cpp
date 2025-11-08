#include "AudioFileRecorder.h"

AudioFileRecorder::AudioFileRecorder(std::filesystem::path savePath)
{
	this->savePath = savePath;

	Name = "AudioFileRecorder";

	NumParameters = AUDIOFILERECORDER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[AUDIOFILERECORDER_SECONDSRECORDED].Name = "SecsRecorded";
	Parameters[AUDIOFILERECORDER_SECONDSRECORDED].SourceVariable = &recordSeconds;
	Parameters[AUDIOFILERECORDER_SECONDSRECORDED].DefaultValue = 0;
	Parameters[AUDIOFILERECORDER_SECONDSRECORDED].SuppressSave = true;
}

AudioFileRecorder::~AudioFileRecorder()
{
	if (waveWriter != nullptr)
		delete waveWriter;
}

void AudioFileRecorder::HandleCommand(std::vector<std::string> commandWords)
{
	if (commandWords.size() > 2)
	{
		if (commandWords[2] == "Save")
		{
			SaveRecording();
		}
	}
}

void AudioFileRecorder::SaveRecording()
{
	time_t t = time(0);
	struct tm now;

#if _WIN32
	localtime_s(&now, &t);
#else
	localtime_r(&t, &now);
#endif

	char filename[80];
	strftime(filename, 80, "%Y-%m-%d-%H%M%S.wav", &now);

	waveWriter->WriteToFile((savePath / filename).string());
	waveWriter->ResetRecording();

	recordSeconds = 0;
}

void AudioFileRecorder::init(int newSamplingFreq)
{
	if (newSamplingFreq != samplingFreq)
	{
		StompBox::init(newSamplingFreq);

		if (waveWriter != nullptr)
		{
			delete waveWriter;

			waveWriter = nullptr;
		}
	}

	if (waveWriter == nullptr)
	{
		recordSeconds = 0;
		waveWriter = new WaveWriter(newSamplingFreq, 60 * 30);
	}
}

void AudioFileRecorder::compute(int count, float* input, float* output)
{
	if (waveWriter != nullptr)
	{
		waveWriter->AddSamples(input, count);

		recordSeconds = waveWriter->GetRecordSeconds();
	}

	for (int i = 0; i < count; i++)
	{
		output[i] = input[i];
	}
}