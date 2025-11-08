#pragma once

#include <list>
#include <stdio.h>

#define ControlChange 0xB0
#define PatchChange 0xC0

struct MidiEvent
{
	int MidiCommand;
	int MidiData1;
	int MidiData2;
	int MidiData2MaxValue;
};

struct SysExCC
{
	unsigned char SysExData[5];
	int DataLength = 4;
	int MidiCommand;
	int MidiData;
	int MaxValue;
};

class SysExMapper
{
protected:
	std::list<SysExCC> ccMap;
	unsigned char sysExHeader[8] = { 0xF0, 0x41, 0x00, 0x00, 0x00, 0x00, 0x30, 0x12 };
public:
	SysExMapper();
	~SysExMapper() { };
	void AddCC(SysExCC cc);
	bool HandleMidiMessage(const unsigned char* data, size_t length, std::list<MidiEvent>& midiEvents);
};