#pragma once
#include <cstring>
#include <cstdint>

#include "ChordDefinitions.h"

namespace Genetics {

	struct Phrase {

		Phrase() 
			: _melodicData(0), _melodicRhythm(0), _melodicNotes(0), _harmonicData(0), 
			  _harmonicNotes(0), _fitnessValue(0.0f), _phraseID(++_phraseCount) {
		}

		Phrase(const Phrase& rhs)
			: _phraseID(++_phraseCount) {

			uint32_t arrayLen = _numMeasures * _smallestSubdivision;
			
			_melodicNotes = rhs._melodicNotes;
			// If the source has melody data, copy it
			if (rhs._melodicData) {
				_melodicData   = new char[arrayLen];
				std::memcpy(_melodicData, rhs._melodicData, arrayLen);
			}
			if (rhs._melodicRhythm) {
				_melodicRhythm = new char[arrayLen];
				std::memcpy(_melodicRhythm, rhs._melodicRhythm, arrayLen);
			}

			_harmonicNotes = rhs._harmonicNotes;
			// If the source has harmony data, copy it
			if (rhs._harmonicData) {
				_harmonicData = new Chord[_harmonicNotes];
				std::memcpy(_harmonicData, rhs._harmonicData, _harmonicNotes * sizeof(Chord));
			}

			_fitnessValue = rhs._fitnessValue;
		}


		~Phrase() {

			if (_melodicData)    { delete[] _melodicData;    }
			if (_melodicRhythm)  { delete[] _melodicRhythm;  }
			if (_harmonicData)   { delete[] _harmonicData;   }
		}

		void reset() {

			int arrayLen = _smallestSubdivision * _numMeasures;
			std::memset(_melodicData, 0, arrayLen);
			std::memset(_melodicRhythm, 0, arrayLen);
			_melodicNotes = 0;
			
			std::memset(_harmonicData, 0, arrayLen * sizeof(Chord));
			_harmonicNotes = 0;
		}

		// Melodic Information
		char*  _melodicData;
		char* _melodicRhythm; // Number of subdivision units
		uint32_t _melodicNotes;
	
		// Harmonic Information
		Chord* _harmonicData; // Stores Roman numeral for current key (C Major default)
		uint32_t _harmonicNotes; // Should be 4 * _numMeasures, chords move every quarter note by default

		// StatisticalInformation
		float _fitnessValue;

		// Identification Information
		uint32_t _phraseID;
		static uint32_t _phraseCount;

		static uint32_t _numMeasures;
		static uint32_t _smallestSubdivision;
	};

	

} // namespace Genetics