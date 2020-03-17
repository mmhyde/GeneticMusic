#include "PhraseConversion.h"

#include "Phrase.h"
#include "PhrasePool.h"

namespace Genetics {

	PhraseConversion::PhraseConversion() 
	{

	}

	PhraseConversion::~PhraseConversion()
	{

	}

	void PhraseConversion::SetMeterInfo(const MIDI::MeterInfo& meterInfo)
	{

	}

	MIDI::Track* PhraseConversion::ConvertToTrack(Phrase* phrase, unsigned measures, unsigned subDiv)
	{
		MIDI::Track* outputTrack = nullptr;

		return outputTrack;
	}

	void PhraseConversion::ConvertToTracks(PhrasePool* phrases, MIDI::Track** trackArray)
	{

	}



} // namespace Genetics