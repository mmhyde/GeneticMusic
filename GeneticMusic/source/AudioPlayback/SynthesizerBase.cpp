
#include "AudioPlayback/SynthesizerBase.h"
#include "AudioPlayback/AudioDefinitions.h"
#include "Phrase.h"

#include <cmath>
#include <iostream>
#include <algorithm>

namespace Genetics {

	constexpr float ATTACK_TIME  = 0.01f;
	constexpr float DECAY_TIME   = 0.05f;
	constexpr float SUSTAIN_GAIN = 0.60f;
	constexpr float RELEASE_TIME = 0.05f;

	constexpr float MIDI_A440    = 69.0f;

#pragma warning(push)
#pragma warning(disable : 4244)

	SynthesizerBase::SynthesizerBase(unsigned rate, const MeterInfo& meterData)
		: m_samplesPerNote(SamplesPerNoteLength(rate, meterData)), m_sampleRate(rate),
		  m_attackT(ATTACK_TIME), m_decayT(DECAY_TIME), m_sustainLevel(SUSTAIN_GAIN), m_releaseT(RELEASE_TIME),
		  m_envelopeArchetype(m_attackT * rate, m_decayT * rate, m_sustainLevel, m_releaseT * rate),
		  m_outputSynth(440.0f, rate, m_envelopeArchetype) {

		// Allocate a temp buffer for generating samples
		m_tempBuffer = new float[m_samplesPerNote * Phrase::_smallestSubdivision];
	}

#pragma warning(pop)

	SynthesizerBase::~SynthesizerBase() {

		// Check memory is valid before calling delete
		if (m_tempBuffer != nullptr) {

			delete[] m_tempBuffer;
			m_tempBuffer = nullptr;
		}
	}

	void SynthesizerBase::Initialize() {
	
	}

	// This function assigns an output array to the outputPtr and returns the number of samples
	uint32_t SynthesizerBase::renderMIDI(float** outputPtr, const Phrase* phrase) {

		// Allocate an output buffer with extra samples on the end for safety
		uint32_t numSamples = m_samplesPerNote * (Phrase::_numMeasures + 1) * Phrase::_smallestSubdivision;
		float* outputBuffer = new float[numSamples];
		uint32_t sampleIndex = 0;

		// Memset to zero to handle silence correctly
		std::memset(outputBuffer, 0, sizeof(float) * Phrase::_numMeasures * Phrase::_smallestSubdivision * m_samplesPerNote);

		// sample length of the chord
		const uint32_t harmonySamples = m_samplesPerNote * ChordRhythm;
		uint8_t previousPitch = 0;

		// Loop over the phrase data
		uint32_t maxIdx = Phrase::_numMeasures * Phrase::_smallestSubdivision;
		for (uint32_t i = 0; i < maxIdx;) {

			// Grab current pitch value
			uint8_t currentPitch = phrase->_melodicData[i];
			uint8_t currentRhythm = phrase->_melodicRhythm[i];

			// sample length of the current melody note
			const uint32_t melodySamples = m_samplesPerNote * currentRhythm;

			// Generate the note as long as it isn't a rest
			if (currentPitch != 0) {

				generateAndAddPitch(outputBuffer + sampleIndex, currentPitch, melodySamples, 0.6f);
			}

			// Check if this is the start of a chord (on a quarter note)
			if (i % ChordRhythm == 0) {

				const Chord& currentChord = phrase->_harmonicData[i / ChordRhythm];

				// If the current melody note is a rest, use the last pitch
				if (currentPitch == 0) { currentPitch = previousPitch; }

				// Calculate root, third, and fifth
				uint8_t root = calculateRootNote(currentPitch, currentChord);
				uint8_t third = root + NumeralSemitones[ChordNumeral::III];
				uint8_t fifth = root + NumeralSemitones[ChordNumeral::  V];

				applyChordType(currentChord._type, third, fifth);

				// Add root to output buffer
				generateAndAddPitch(outputBuffer + sampleIndex, root , harmonySamples, 0.30f);

				// Add third to output buffer
				generateAndAddPitch(outputBuffer + sampleIndex, third, harmonySamples, 0.15f);

				// Add fifth to output buffer
				generateAndAddPitch(outputBuffer + sampleIndex, fifth, harmonySamples, 0.15f);

				// Set pitch history
				previousPitch = currentPitch;
			}

			// Increment phrase note and sample index by the smaller of the two rhythms; ensures we don't miss any notes
			if (currentRhythm == 0) {

				i += ChordRhythm;
				sampleIndex += ChordRhythm;
			}
			else {
		
				i += std::min(ChordRhythm, currentRhythm);
				sampleIndex += std::min(melodySamples, harmonySamples);
			}
		}

		for (uint32_t i = 0; i < sampleIndex; ++i) {

			outputBuffer[i] *= 0.5f;
		}

		*outputPtr = outputBuffer;
		return sampleIndex;
	}

	void SynthesizerBase::generateAndAddPitch(float* output, uint8_t pitch, uint32_t numSamples, float gain) {

		// Determine frequency of root note and generate
		double exponent = static_cast<double>(pitch - MIDI_A440) / static_cast<double>(OctaveInterval);
		float frequency = 440.0f * static_cast<float>(std::pow(2.0, exponent));

		m_outputSynth.SetFrequency(frequency, m_sampleRate);
		m_outputSynth.GenerateSamples(m_tempBuffer, numSamples);

		// Add root to output buffer
		for (uint32_t sample = 0; sample < numSamples; ++sample) {

			output[sample] += (m_tempBuffer[sample] * gain);
		}
	}

	const double PI = 4.0 * std::atan(1.0);

	SynthesizerBase::SineVoice::SineVoice(float frequency, const unsigned& sampleRate, const Envelope& envelopeSource)
		: m_envelope(envelopeSource)
	{
		SineHelper(frequency, sampleRate);
	}

	void SynthesizerBase::SineVoice::GenerateSamples(float* samples, unsigned numSamples)
	{
		for (unsigned i = 0; i < numSamples; ++i)
		{
			samples[i] = y_0;
			float y_2 = m_sineAlpha * y_1 - y_0;
			y_0 = y_1;
			y_1 = y_2;
		}

		//samples -= numSamples;
		m_envelope.ApplyGain(samples, numSamples);
	}

	void SynthesizerBase::SineVoice::SetFrequency(float frequency, const unsigned& sampleRate)
	{
		SineHelper(frequency, sampleRate);
	}

	void SynthesizerBase::SineVoice::SineHelper(float frequency, const unsigned& sampleRate)
	{
		double coefficient = (2.0 * PI * frequency) / sampleRate;

		y_0 = 0.0f;
		y_1 = static_cast<float>(std::sin(coefficient));
		m_sineAlpha = 2.0f * static_cast<float>(std::cos(coefficient));
	}

	Envelope::Envelope(unsigned attackS, unsigned decayS, float susLevel, unsigned releaseS)
		: m_attackSamples(attackS), m_decaySamples(decayS),
		m_sustainLevel(susLevel), m_releaseSamples(releaseS)
	{
	}

	Envelope::Envelope(const Envelope& source)
		: m_attackSamples(source.m_attackSamples), m_decaySamples(source.m_decaySamples),
		m_sustainLevel(source.m_sustainLevel), m_releaseSamples(source.m_releaseSamples)
	{
	}

	void Envelope::ApplyGain(float* samples, unsigned numSamples)
	{
		float attackInc = 1.0f / m_attackSamples;
		float decayInc = (1.0f - m_sustainLevel) / m_decaySamples;
		float releaseInc = m_sustainLevel / m_releaseSamples;
		float gain = 0.0f;

		unsigned currentSample = 0;

		while (currentSample < numSamples - m_releaseSamples)
		{
			// Attack step
			if (currentSample < m_attackSamples) {
				*samples++ *= gain;
				gain += attackInc;
			}
			// Decay step
			else if (currentSample < m_attackSamples + m_decaySamples)
			{
				*samples++ *= gain;
				gain -= decayInc;
			}
			// Sustain step
			else
			{
				*samples++ *= gain;
			}

			++currentSample;
		}

		while (currentSample < numSamples) {
			*samples++ *= gain;
			gain -= releaseInc;
			++currentSample;
		}

		samples -= numSamples;
	}

} // namespace Genetics
