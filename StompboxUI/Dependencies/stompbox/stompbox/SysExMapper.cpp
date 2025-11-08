#include "SysExMapper.h"

SysExMapper::SysExMapper()
{
}

void SysExMapper::AddCC(SysExCC cc)
{
	ccMap.push_back(cc);
}

bool SysExMapper::HandleMidiMessage(const unsigned char* data, size_t length, std::list<MidiEvent>& midiEvents)
{
	bool matched = false;

	if (length < (sizeof(sysExHeader) + 6))
	{
		// Data not long enough
	}
	else
	{
		for (size_t i = 0; i < sizeof(sysExHeader); i++)
		{
			if (data[i] != sysExHeader[i])
				return false;
		}

		for (const auto& cc : ccMap)
		{
			bool thisMatched = true;

			for (int i = 0; i < cc.DataLength; i++)
			{
				if (cc.SysExData[i] != data[sizeof(sysExHeader) + i])
				{
					thisMatched = false;

					break;
				}
			}

			if (thisMatched)
			{
				int sysExValue = data[sizeof(sysExHeader) + cc.DataLength];

				MidiEvent event;
				event.MidiCommand = cc.MidiCommand;

				if (cc.MidiData == -1)
				{
					event.MidiData1 = sysExValue;
					event.MidiData2 = 0;
				}
				else
				{
					event.MidiData1 = cc.MidiData;
					event.MidiData2 = sysExValue;
					event.MidiData2MaxValue = cc.MaxValue;
				}

				midiEvents.push_back(event);

				matched = true;
			}
		}
	}

	return matched;
}
