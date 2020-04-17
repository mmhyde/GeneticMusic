#pragma once

#include <cstdint>

namespace Genetics {

	enum ChordType {

		Major = 0,
		Minor,
		Augmented,
		Diminished
	};

	// Roman numerals used to represent chords
	enum ChordNumeral {

		I = 0,
		II,
		III,
		IV,
		V,
		VI,
		VII
	};

	constexpr uint8_t NumeralSemitones[] = { 0, 2, 4, 5, 7, 9, 11 };

	// Chord structure
	struct Chord {
		Chord(uint8_t numeral = ChordNumeral::I)
			: _numeral(numeral), _type(ChordType::Major) {
		}

		uint8_t _numeral : 4;
		uint8_t _type : 4;
	};

	constexpr uint8_t MiddleC = 60;
	constexpr uint8_t OctaveInterval = 12;
	constexpr uint8_t ChordNoteLen = 4;
	constexpr uint8_t ChordRhythm = 16 / ChordNoteLen;

	__inline uint8_t calculateRootNote(const uint8_t pitch, const Chord& chord) {

		// Determine root note of the chord
		uint8_t rootNote = MiddleC + NumeralSemitones[chord._numeral];

		// Shift root to be within an octave lower than the current pitch
		// Interval (pitch - root) should be: [11, 0]

		// Shifting root down
		while (pitch < rootNote) {
			rootNote -= OctaveInterval;
		}

		// Shifting root up
		while (pitch - OctaveInterval >= rootNote) {
			rootNote += OctaveInterval;
		}

		return rootNote;
	}

	// Applies the specified chord type the third and fifth of the chord, it's assumed the passed in notes 
	// are the Major third and Major fifth above the root
	__inline void applyChordType(const uint8_t chordType, uint8_t& third, uint8_t& fifth) {

		if (chordType == ChordType::Major)           { // Major Third, Major Fifth
			
			// Leave third alone, already Major
			// Leave fifth alone, already Major
		}
		else if (chordType == ChordType::Minor)      { // minor Third, Major Fifth

			third -= 1; // Convert third to minor
			// Leave fifth alone, already Major
		}
		else if (chordType == ChordType::Augmented)  { // Major Third, Augmented Fifth

			// Leave third alone, already Major
			fifth += 1; // Convert fifth to augmented
		}
		else if (chordType == ChordType::Diminished) { // minor Third, diminished Fifth

			third -= 1; // Convert third to minor
			fifth -= 1; // Convert fifth to diminished 
		}
	}

} // namespace Genetics