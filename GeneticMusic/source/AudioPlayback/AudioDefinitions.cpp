
#include "AudioPlayback/AudioDefinitions.h"

namespace Genetics {

	MeasureNumber GetMeasureAtTime(TimeStamp time, const MeterInfo& meterInfo)
	{
		double measureFrac = ((double)meterInfo.beatsPerMinute * time) / ((double)meterInfo.beatsPerBar * 60.0);
		return static_cast<MeasureNumber>(measureFrac);
	}

	TimeStamp GetTimeAtMeasure(MeasureNumber measureNum, const MeterInfo& meterInfo)
	{
		return ((double)(measureNum * meterInfo.beatsPerBar) * 60.0) / (double)meterInfo.beatsPerMinute;
	}

	unsigned short GetNotesPerBar(const MeterInfo& meterInfo)
	{
		return (NoteLength / meterInfo.beatNote) * meterInfo.beatsPerBar;
	}

	unsigned int SamplesPerNoteLength(unsigned int sampleRate, const MeterInfo& meterInfo)
	{
		unsigned int samples = 60 * sampleRate * meterInfo.beatNote;
		return samples / (NoteLength * meterInfo.beatsPerMinute);
	}

} // namespace Genetics
