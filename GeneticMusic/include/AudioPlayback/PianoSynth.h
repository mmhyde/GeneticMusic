#pragma once

#include "AudioPlayback/SynthesizerBase.h"

#include <fstream>
#include <iostream>
#include <cstdint>
#include <vector>

namespace Genetics {

	struct MeterInfo;
	struct Phrase;

	class PianoSynth : public SynthesizerBase {

	public:
		PianoSynth(uint32_t rate, const MeterInfo& meterData);
		~PianoSynth();

		virtual uint32_t renderMIDI(float** outputPtr, const Phrase* notes);

	private:
		void loadFileToArray();
		void generateAndAdd(float* output, uint8_t pitch, uint32_t numSamples, float gain = 1.0f);

		struct PianoSample {
			PianoSample(float* samples, uint32_t sampleLength, const Envelope& envelopeSource);
			
			void generateSamples(float* samples, uint32_t numSamples);

			float* m_samples;
			uint32_t m_arrLen;

			Envelope m_envSource;
		};

		float m_attackT;
		float m_decayT;
		float m_sustainLevel;
		float m_releaseT;

		Envelope m_envelopeArchetype;
		std::vector<PianoSample> m_sampleTable;// Sample lib is 88 notes

		float* m_audioFileData;
		uint32_t m_fileSampleCount;

		float* m_tempBuffer;

		const unsigned m_sampleRate;
		const unsigned m_samplesPerNote;
	};



} // namespace Genetics