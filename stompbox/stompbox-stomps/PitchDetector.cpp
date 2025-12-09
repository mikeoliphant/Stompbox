#include <math.h>
#include <algorithm>
#include "PitchDetector.h"

PitchDetector::PitchDetector(int bufferSize)
{
	this->bufferSize = bufferSize;
	this->bufferSizeTimesTwo = bufferSize * 2;

	Name = "PitchDetector";

	NumParameters = PITCHDETECTOR_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[PITCHDETECTOR_MUTE].Name = "Mute";
	Parameters[PITCHDETECTOR_MUTE].SourceVariable = &muteOutput;
	Parameters[PITCHDETECTOR_MUTE].IsAdvanced = true;
	Parameters[PITCHDETECTOR_MUTE].ParameterType = PARAMETER_TYPE_BOOL;
	Parameters[PITCHDETECTOR_MUTE].MinValue = 0;
	Parameters[PITCHDETECTOR_MUTE].MaxValue = 1;
	Parameters[PITCHDETECTOR_MUTE].DefaultValue = 1;

	Parameters[PITCHDETECTOR_PITCH].Name = "Pitch";
	Parameters[PITCHDETECTOR_PITCH].SourceVariable = &currentPitch;
	Parameters[PITCHDETECTOR_PITCH].IsOutput = true;
	Parameters[PITCHDETECTOR_PITCH].MinValue = 0;
	Parameters[PITCHDETECTOR_PITCH].MaxValue = 10000;
	Parameters[PITCHDETECTOR_PITCH].DefaultValue = currentPitch;

	pitchMPM = new PitchMPM(bufferSize);
	buffer = new float[bufferSizeTimesTwo];

	bufIndex = 0;
}

float PitchDetector::GetCurrentPitch()
{
	return (float)currentPitch;
}

void PitchDetector::init(int newSamplingFreq)
{
	StompBox::init(newSamplingFreq);

	pitchMPM->setSampleRate(newSamplingFreq);
}

long sampsSoFar = 0;

void PitchDetector::compute(int count, float* input0, float* output0)
{
	for (int i = 0; i < count; i++)
	{
		float val = (float)input0[i];

		buffer[(bufIndex + bufferSize) % bufferSizeTimesTwo] = val;
		buffer[bufIndex++] = val;

		output0[i] = (muteOutput == 1) ? 0 : input0[i];

		if (bufIndex == bufferSize)
		{
			bufIndex = 0;
		}
	}

	int offset = bufIndex - bufferSize;
	
	if (offset < 0)
		offset += bufferSize;

	sampsSoFar += bufferSize;

	if (sampsSoFar > (samplingFreq / 10))
	{
		currentPitch = pitchMPM->getPitch(buffer + offset);

		sampsSoFar = 0;
	}

	//if (bufIndex == bufferSize)
	//{
	//	currentPitch = pitchMPM->getPitch(buffer);

		//if (currentPitch > 0)
		//{
		//	float note = (log(currentPitch / 440.0) / log(2) * 12 + 69) + 12;

		//	note = round(note);

		//	if (!noteOn || (lastNote != note))
		//	{
		//		if (noteOn)
		//			MidiCallback(0x80, lastNote, 0);

		//		MidiCallback(0x90, note, 127);

		//		noteOn = true;
		//		lastNote = note;
		//	}
		//}
		//else
		//{
		//	if (noteOn)
		//	{
		//		MidiCallback(0x80, lastNote, 0);

		//		noteOn = false;
		//	}
		//}

		//float pitch = pitchMPM->getPitch(buffer);

		//if (pitch > 0)
		//{
		//	int factor = std::min(10, numDetects);

		//	currentPitch = ((factor * currentPitch) + pitch) / (double)(factor + 1);

		//	numDetects++;
		//}
		//else
		//{
		//	numFailures++;

		//	if (numFailures > 10)
		//	{
		//		numDetects = 0;
		//		currentPitch = -1;
		//	}
		//}

	//	bufIndex = 0;
	//}
}