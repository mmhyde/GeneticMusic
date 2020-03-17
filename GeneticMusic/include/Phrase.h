#pragma once
#include <cstring>
#include <cstdint>

namespace Genetics {

	// Chord structure
	struct Chord
	{
		char _root    : 8; // Root note
		char _3rdDiff : 4; // Semitones to 3rd from Root
		char _5thDiff : 4; // Semitones to 5th from 3rd
	};

	struct Phrase
	{
		Phrase() 
			: _melodicData(0), _melodicRhythm(0), _melodicNotes(0),
			  _harmonicData(0), _harmonicRhythm(0), _harmonicNotes(0),
			  _fitnessValue(0.5f), _phraseID(++_phraseCount)
		{
		}

		~Phrase()
		{
			if (_melodicData)    { delete[] _melodicData;    }
			if (_melodicRhythm)  { delete[] _melodicRhythm;  }
			if (_harmonicData)   { delete[] _harmonicData;   }
			if (_harmonicRhythm) { delete[] _harmonicRhythm; }
		}

		void reset() {

			int arrayLen = _smallestSubdivision * _numMeasures;
			std::memset(_melodicData, 0, arrayLen);
			std::memset(_melodicRhythm, 0, arrayLen);
			_melodicNotes = 0;
			
			//std::memset(_harmonicData, 0, arrayLen * sizeof(Chord));
			//std::memset(_harmonicLength, 0, arrayLen);
			//_harmonicNotes = 0;
		}

		// Melodic Information
		char*  _melodicData;
		char* _melodicRhythm; // Number of subdivision units
		uint32_t _melodicNotes;
	
		// Harmonic Information
		Chord* _harmonicData;
		char* _harmonicRhythm; // Number of subdivision units
		uint32_t _harmonicNotes;

		// StatisticalInformation
		float _fitnessValue;

		// Identification Information
		uint32_t _phraseID;
		static uint32_t _phraseCount;

		static uint32_t _numMeasures;
		static uint32_t _smallestSubdivision;
	};

	

} // namespace Genetics