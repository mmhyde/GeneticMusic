#pragma once

#include <cstdint>

namespace Genetics {

	struct MeterInfo;
	struct Phrase;

	class Envelope
	{
	public:

		Envelope(unsigned attackS, unsigned decayS, float susLevel, unsigned releaseS);
		Envelope(const Envelope& source);

		void ApplyGain(float* samples, unsigned numSamples);

	private:

		unsigned m_attackSamples;
		unsigned m_decaySamples;
		float m_sustainLevel;
		unsigned m_releaseSamples;
	};

	class SynthesizerBase
	{
	public:
		SynthesizerBase(unsigned sampleRate, const MeterInfo& meterData);
		virtual ~SynthesizerBase();

		// This function assigns an output array to the outputPtr and returns the number of samples
		virtual uint32_t renderMIDI(float** outputPtr, const Phrase* noteList);

	private:
		void generateAndAddPitch(float* output, uint8_t pitch, uint32_t numSamples, float gain = 1.0f);

		// Sine wave generator
		struct SineVoice {
			SineVoice(float frequency, const unsigned& sampleRate, const Envelope& envelopeSource);

			void GenerateSamples(float* samples, unsigned numSamples);
			void SetFrequency(float frequency, const unsigned& sampleRate);

			void SineHelper(float frequency, const unsigned& sampleRate);

			Envelope m_envelope;

			float y_0, y_1;
			float m_sineAlpha;
		};

		// Envelope times
		float m_attackT;
		float m_decayT;
		float m_sustainLevel;
		float m_releaseT;

		Envelope m_envelopeArchetype;
		SineVoice m_outputSynth;

		float* m_tempBuffer;

		const unsigned m_sampleRate;
		const unsigned m_samplesPerNote;
	};

} // namespace Genetics