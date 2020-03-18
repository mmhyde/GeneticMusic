
#include "AudioPlayback/SynthesizerBase.h"
#include "AudioPlayback/AudioDefinitions.h"
#include "Phrase.h"

#include <cmath>
#include <iostream>

namespace Genetics
{

	constexpr float ATTACK_TIME  = 0.01f;
	constexpr float DECAY_TIME   = 0.05f;
	constexpr float SUSTAIN_GAIN = 0.75f;
	constexpr float RELEASE_TIME = 0.05f;

#pragma warning(push)
#pragma warning(disable : 4244)

	SynthesizerBase::SynthesizerBase(unsigned rate, const MeterInfo& meterData)
		: m_samplesPerNote(SamplesPerNoteLength(rate, meterData)), m_sampleRate(rate),
		m_attackT(ATTACK_TIME), m_decayT(DECAY_TIME), m_sustainLevel(SUSTAIN_GAIN), m_releaseT(RELEASE_TIME),
		m_envelopeArchetype(m_attackT * rate, m_decayT * rate, m_sustainLevel, m_releaseT * rate),
		m_outputSynth(440.0f, rate, m_envelopeArchetype)
	{
	}

#pragma warning(pop)

	SynthesizerBase::~SynthesizerBase()
	{
	}

	void SynthesizerBase::Initialize()
	{
	}

	// This function assigns an output array to the outputPtr and returns the number of samples
	unsigned SynthesizerBase::RenderMIDI(float** outputPtr, const Phrase* phrase, 
		                                 unsigned measureCount, unsigned subdivision)
	{
		// Allocate an output buffer with extra samples on the end for safety
		unsigned numSamples = m_samplesPerNote * (measureCount + 1) * subdivision;
		float* outputBuffer = new float[numSamples];
		// Memset to zero to handle silence correctly
		std::memset(outputBuffer, 0, sizeof(float) * measureCount * subdivision * m_samplesPerNote);

		unsigned maxIndex = measureCount * subdivision;
		unsigned sampleIndex = 0;
		for (unsigned i = 0; i < maxIndex;)
		{
			// Grab the current pitch value
			unsigned char currPitch = phrase->_melodicData[i];
			unsigned char currLength = phrase->_melodicRhythm[i];

			// Move index forward by the number 
			i += currLength;

			unsigned noteSamples = m_samplesPerNote * currLength;

			// If this note is a rest just move the index along and skip over the rest of the loop
			if (currPitch == 0) {
				sampleIndex += noteSamples;
				continue;
			}

			// Calculate the frequency of the next note
			float frequency = 440.0f * static_cast<float>(std::pow(2.0, (double)(currPitch - 69.0) / 12.0));

			// Set the frequency on the synth and generate the samples
			m_outputSynth.SetFrequency(frequency, m_sampleRate);
			m_outputSynth.GenerateSamples(outputBuffer + sampleIndex, noteSamples);

			// Update index variables
			sampleIndex += noteSamples;
		}
		
		*outputPtr = outputBuffer;
		return sampleIndex;
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
