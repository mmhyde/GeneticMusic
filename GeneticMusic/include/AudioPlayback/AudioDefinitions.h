#pragma once

namespace Genetics {

	typedef unsigned short MeasureNumber;
	typedef double TimeStamp;
	typedef unsigned short Tempo;

	constexpr Tempo DefaultTempo = 120;

	constexpr unsigned char NoteLength = 16;

	struct MeterInfo {
		Tempo beatsPerMinute = DefaultTempo;
		unsigned char  beatsPerBar;
		unsigned char  beatNote;
	};

	MeasureNumber GetMeasureAtTime(TimeStamp time, const MeterInfo& meterInfo);

	TimeStamp GetTimeAtMeasure(MeasureNumber measureNum, const MeterInfo& meterInfo);

	unsigned short GetNotesPerBar(const MeterInfo& meterInfo);

	unsigned int SamplesPerNoteLength(unsigned int sampleRate, const MeterInfo& meterInfo);

} // namespace Genetics