#include "Fitness/RuleExtractors.h"
#include "Fitness/RuleBuilder.h"

#include "Phrase.h"
#include <iostream>

namespace Genetics {


	PitchExtractor::PitchExtractor(const RuleList<PitchRule> rules)
		: m_pitchRules(rules), m_dataBuffer(new char[Phrase::_numMeasures * Phrase::_smallestSubdivision]) {

	}

	PitchExtractor::~PitchExtractor() {

		delete[] m_dataBuffer;
	}

	float PitchExtractor::process(Phrase* subject) const {

		float total = 0.0f;
		std::memset(m_dataBuffer, 0, sizeof(char) * Phrase::_numMeasures * Phrase::_smallestSubdivision);

		uint32_t note = 0;
		for (uint32_t i = 0; i < subject->_melodicNotes; ++i) {

			m_dataBuffer[i] = subject->_melodicData[note];
			note += subject->_melodicRhythm[note];
		}

		return m_pitchRules.evaluateAll(m_dataBuffer, subject->_melodicNotes);
	}



	RhythmExtractor::RhythmExtractor(const RuleList<RhythmRule> rules)
		: m_rhythmRules(rules), m_dataBuffer(new char[Phrase::_numMeasures * Phrase::_smallestSubdivision]) {

	}

	RhythmExtractor::~RhythmExtractor() {

		delete[] m_dataBuffer;
	}

	float RhythmExtractor::process(Phrase* subject) const {

		float total = 0.0f;
		std::memset(m_dataBuffer, 0, sizeof(char) * Phrase::_numMeasures * Phrase::_smallestSubdivision);

		uint32_t note = 0;
		for (uint32_t i = 0; i < subject->_melodicNotes; ++i) {

			// Build packed array of note lengths
			m_dataBuffer[i] = subject->_melodicRhythm[note];
			note += subject->_melodicRhythm[note];
		}

		// Run every function for rhythm and return the values
		return m_rhythmRules.evaluateAll(m_dataBuffer, subject->_melodicNotes);
	}



	IntervalExtractor::IntervalExtractor(const RuleList<IntervalRule> rules)
		: m_intervalRules(rules), m_dataBuffer(new uint8_t[Phrase::_numMeasures * Phrase::_smallestSubdivision]) {

	}

	IntervalExtractor::~IntervalExtractor() {

		delete[] m_dataBuffer;
	}

	float IntervalExtractor::process(Phrase* subject) const {

		float total = 0.0f;
		std::memset(m_dataBuffer, 0, sizeof(uint8_t) * Phrase::_numMeasures * Phrase::_smallestSubdivision);

		uint32_t note = subject->_melodicRhythm[0];
		char lastPitch = subject->_melodicData[0];
		for (uint32_t i = 1; i < subject->_melodicNotes; ++i) {

			// Calculate intervals between each pair of notes
			m_dataBuffer[i - 1] = std::abs(lastPitch - subject->_melodicData[note]);
			lastPitch = subject->_melodicData[note];
			note += subject->_melodicRhythm[note];
		}

		return m_intervalRules.evaluateAll(m_dataBuffer, subject->_melodicNotes - 1);
	}



	MeasureExtractor::MeasureExtractor(const RuleList<MeasureRule> rules)
		: m_measureRules(rules), m_dataBuffer(new Measure[Phrase::_numMeasures]) {
	}

	MeasureExtractor::~MeasureExtractor() {

		delete[] m_dataBuffer;
	}

	float MeasureExtractor::process(Phrase* subject) const {

		

		return 0.0f;
	}

	ChordExtractor::ChordExtractor(const RuleList<ChordRule> rules)
		: m_chordRules(rules), m_dataBuffer(new uint8_t[Phrase::_numMeasures * Phrase::_smallestSubdivision]) {
	}

	ChordExtractor::~ChordExtractor() {
		
		delete[] m_dataBuffer;
	}

	float ChordExtractor::process(Phrase* subject) const {

		float total = 0.0f;
		std::memset(m_dataBuffer, 0, sizeof(uint8_t) * Phrase::_numMeasures * Phrase::_smallestSubdivision);

		uint32_t noteIdx = 0;

		uint8_t previousPitch = 0, currentRoot = 0;
		for (uint32_t outputIdx = 0; outputIdx < subject->_melodicNotes; ++outputIdx) {

			// Get the pitch of the next note
			uint8_t pitch = subject->_melodicData[noteIdx];

			// If the pitch is zero use the previous one
			if (pitch == 0) { pitch = previousPitch; }

			// Get the current root note
			if (noteIdx % 4 == 0) {
				const Chord& currentChord = subject->_harmonicData[noteIdx / ChordRhythm];
				currentRoot = calculateRootNote(pitch, currentChord);
			}

			// Determine interval between the root and current pitch
			m_dataBuffer[outputIdx] = pitch - currentRoot;
			
			// Determine index shift amounts
			noteIdx += subject->_melodicRhythm[noteIdx];
		}

		return m_chordRules.evaluateAll(m_dataBuffer, subject->_harmonicNotes);
	}

} // namespace Genetics