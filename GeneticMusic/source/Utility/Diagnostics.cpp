
#include "Utility/Diagnostics.h"
#include "Phrase.h"

#include <iostream>


namespace Genetics {

	enum GA_ErrorCodes {

		enm_noError = 0,
		enm_noteCountMismatch,
		enm_noteLengthTooShort,
		enm_noteLengthTooLong,
		enm_restFound,
	};

	std::string readErrorMessage(GA_Error errorCode) {

		uint16_t errorNum = errorCode >> 16;

		switch (errorCode) {

		case enm_noError:
			return "No error";

		case enm_noteCountMismatch:
			return "Expected note count differed from actual count";
		
		case enm_noteLengthTooShort:
			return "Failed to find a note where one was expected (Hole in the measure)";

		case enm_noteLengthTooLong:
			return "Found a note before one was expected to occur (Overlapping notes)";

		case enm_restFound:
			return "Found a rest note where one wasn't expected";

		default:
			return "Unrecognized error code";
		}
	}

	void printErrorMessage(GA_Error errorCode) {

		std::cout << readErrorMessage(errorCode) << std::endl;
	}


	GA_Error validateNoteCount(Phrase* phrase) {

		uint32_t arrayLen = Phrase::_numMeasures * Phrase::_smallestSubdivision;
		uint32_t noteCount = 0;

		for (uint32_t i = 0; i < arrayLen; ++i) {

			if (phrase->_melodicRhythm[i] > 0) {
				++noteCount;
			}
		}

		if (noteCount == phrase->_melodicNotes) {
			
			return enm_noError;
		}
		else {

			return enm_noteCountMismatch;
		}
	}

	GA_Error validateNoteLengths(Phrase* phrase) {

		uint32_t arrayLen = Phrase::_numMeasures * Phrase::_smallestSubdivision;
		uint32_t remainingIndices = 0;

		for (uint32_t i = 0; i < arrayLen; ++i) {

			// If we find a note
			if (phrase->_melodicRhythm[i] != 0) {
				
				if (remainingIndices == 0) {

					// Set the expected number of zeros in the array before we find another note
					remainingIndices = phrase->_melodicRhythm[i] - 1;
				}
				else {

					// Discovered a note where we didn't expect one, fail immediately
					return enm_noteLengthTooLong;
				}
			}
			// No note at this index
			else {

				// Did we expect to find a note here
				if (remainingIndices == 0) {
					
					// Failed to find a note where we expected one
					return enm_noteLengthTooShort;
				}
				else {

					// As expected just decrement the counter
					--remainingIndices;
				}
			}
		}

		return enm_noError;
	}

	GA_Error validateRestOccurances(Phrase* phrase) {

		uint32_t arrayLen = Phrase::_numMeasures * Phrase::_smallestSubdivision;
		
		for (uint32_t i = 0; i < arrayLen; ++i) {

			// If the current pitch is a zero the current rhythm value should 
			// also be zero or there's an error
			if (phrase->_melodicData[i] == 0 && phrase->_melodicRhythm[i] != 0) {

				return enm_restFound;
			}
		}

		return enm_noError;
	}

} // namespace Genetics
