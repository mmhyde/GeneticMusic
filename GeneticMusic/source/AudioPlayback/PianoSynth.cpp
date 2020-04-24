
#include "AudioPlayback/PianoSynth.h"
#include "AudioPlayback/FileDefinitions.h"
#include "AudioPlayback/Int24.h"
#include "AudioPlayback/AudioDefinitions.h"

#include "ChordDefinitions.h"
#include "Phrase.h"

#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace Genetics {

	constexpr float ATTACK_TIME = 0.10f;
	constexpr float DECAY_TIME = 0.01f;
	constexpr float SUSTAIN_GAIN = 0.75f;
	constexpr float RELEASE_TIME = 0.03f;

	constexpr float MIDI_A440 = 69.0f;

	constexpr uint16_t noteCount = 88;
	const std::string samplePath = "Assets/PianoSamplesMono.wav";

	const double PI = 4.0f * std::atan(1.0);

#pragma warning(push)
#pragma warning(disable : 4244)

	PianoSynth::PianoSynth(uint32_t rate, const MeterInfo& meterData)
		: m_samplesPerNote(SamplesPerNoteLength(rate, meterData)), m_sampleRate(rate), SynthesizerBase(rate, meterData),
		m_attackT(ATTACK_TIME), m_decayT(DECAY_TIME), m_sustainLevel(SUSTAIN_GAIN), m_releaseT(RELEASE_TIME), 
		m_envelopeArchetype(m_attackT * rate, m_decayT * rate,  m_sustainLevel, m_releaseT * rate) {
		
		m_tempBuffer = new float[m_samplesPerNote * Phrase::_smallestSubdivision];

		// Load sampler file into memory
		loadFileToArray();

		// Each note is 6 seconds long which is 48000 * 6 samples
		uint32_t noteStride = 48000 * 6;

		// Reserve space for all 88 sampler notes
		m_sampleTable.reserve(noteCount);

		// Loop over all notes in the file
		for (uint16_t i = 0; i < noteCount; ++i) {

			// Determine sample offset of the current note
			float* noteStart =  m_audioFileData + i * noteStride;

			// Emplace the sample struct into the vector
			m_sampleTable.emplace_back(noteStart, noteStride, m_envelopeArchetype);
		}
	}

#pragma warning(pop)

	PianoSynth::~PianoSynth() {

		delete[] m_audioFileData;
		m_audioFileData = nullptr;

		delete[] m_tempBuffer;
		m_tempBuffer = nullptr;
	}

	uint32_t PianoSynth::renderMIDI(float** outputPtr, const Phrase* phrase) {

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

				generateAndAdd(outputBuffer + sampleIndex, currentPitch, melodySamples, 1.2f);
			}
			
			// Check if this is the start of a chord (on a quarter note)
			if (i % ChordRhythm == 0) {

				const Chord& currentChord = phrase->_harmonicData[i / ChordRhythm];

				// If the current melody note is a rest, use the last pitch
				if (currentPitch == 0) { currentPitch = previousPitch; }

				// Calculate root, third, and fifth
				uint8_t root = calculateRootNote(currentPitch, currentChord);
				uint8_t third = root + NumeralSemitones[ChordNumeral::III];
				uint8_t fifth = root + NumeralSemitones[ChordNumeral::V];

				applyChordType(currentChord._type, third, fifth);

				// Add root to output buffer
				generateAndAdd(outputBuffer + sampleIndex, root, harmonySamples, 0.75f);

				// Add third to output buffer
				generateAndAdd(outputBuffer + sampleIndex, third, harmonySamples, 0.75f);

				// Add fifth to output buffer
				generateAndAdd(outputBuffer + sampleIndex, fifth, harmonySamples, 0.75f);

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

			outputBuffer[i] *= 1.40f;
		}

		*outputPtr = outputBuffer;
		return sampleIndex;
	}

	void PianoSynth::loadFileToArray() {

		std::ifstream samplerFile(samplePath, std::ios_base::binary);
		if (!samplerFile.is_open()) {

			std::cout << "Failed to open piano sampler asset" << std::endl;
			return;
		}

		RIFFChunk actualRiff;
		fmt_Chunk actualFmt;

		samplerFile.read(reinterpret_cast<char*>(&actualRiff), sizeof(RIFFChunk));
		if (memcmp(actualRiff.ID, expectedRiff.ID, 4) != 0) {

			std::cout << std::string("Failed to load " + samplePath + ", missing RIFF Tag").c_str() << std::endl;
		}

		if (memcmp(actualRiff.type, expectedRiff.type, 4) != 0) {

			std::cout << std::string("Failed to load " + samplePath + ", missing WAVE Tag").c_str() << std::endl;
		}

		char readHeader[8];
		samplerFile.read(readHeader, CHUNK_HEADER_SIZE);
		while (memcmp(readHeader, expectedFmt.ID, 4) != 0 && !samplerFile.eof()) {

			int ignoreSize = *reinterpret_cast<int*>(readHeader + 4);
			samplerFile.ignore(ignoreSize);
			samplerFile.read(readHeader, CHUNK_HEADER_SIZE);
		}

		if (samplerFile.eof()) {

			std::cout << std::string("Failed to read file " + samplePath + ", no format chunk").c_str() << std::endl;
		}

		int formatSize = *reinterpret_cast<int*>(readHeader + 4);
		samplerFile.read(reinterpret_cast<char*>(&(actualFmt.format)), formatSize);
		short bitDepth = actualFmt.bitsPerSample;

		std::cout << "Sample Rate: " << actualFmt.sampleRate << std::endl;

		samplerFile.read(readHeader, CHUNK_HEADER_SIZE);
		while (memcmp(readHeader, expectedData.ID, 4) != 0 && !samplerFile.eof()) {

			int fakeChunkSize = *reinterpret_cast<int*>(readHeader + 4);
			samplerFile.ignore(fakeChunkSize);
			samplerFile.read(readHeader, CHUNK_HEADER_SIZE);
		}

		if (samplerFile.eof()) {

			std::cout << std::string("Failed to read file " + samplePath + ", no data chunk").c_str() << std::endl;
		}
		int dataSize = *reinterpret_cast<int*>(readHeader + 4);
		int numSamples = dataSize / (bitDepth / 8);

		float* data = nullptr;

		if (bitDepth == 16) {

			short* shortData = new short[dataSize / sizeof(short)];
			samplerFile.read(reinterpret_cast<char*>(shortData), dataSize);
			data = new float[numSamples];

			for (int i = 0; i < numSamples; ++i) {

				data[i] = *shortData / MAX_16;
				shortData++;
			}

			shortData -= numSamples;
			delete[] shortData;
		}
		else if (bitDepth == 24) {

			std::streamsize startSize = samplerFile.gcount();

			Int24* integer24Data = new Int24[dataSize / sizeof(Int24)];
			samplerFile.read(reinterpret_cast<char*>(integer24Data), dataSize);

			std::streamsize endSize = samplerFile.gcount();

			std::cout << "Read " << endSize - startSize << " of expected " << dataSize << " bytes" << std::endl;

			if (samplerFile.rdstate() & samplerFile.failbit) {
				std::cout << "Failed to read the required number of characters" << std::endl;
			}

			if (samplerFile.eof()) {
				std::cout << "End of file bit set for some fucking reason" << std::endl;
			}

			if (actualFmt.channels == 1) {

				data = new float[numSamples];

				for (int i = 0; i < numSamples; ++i) {

					data[i] = static_cast<int>(*integer24Data) / MAX_24;
					++integer24Data;
				}
			}
			else if (actualFmt.channels == 2) {

				data = new float[numSamples / 2];

				float halfPower = std::sin(PI / 4.0);

				for (int i = 0; i < numSamples / 2; ++i) {

					float left = halfPower * static_cast<int>(*integer24Data) / MAX_24;
					++integer24Data;

					float right = halfPower * static_cast<int>(*integer24Data) / MAX_24;
					++integer24Data;

					data[i] = left + right;
				}
			}

			integer24Data -= numSamples;
			delete[] integer24Data;
		}
		else if (bitDepth == 32) {

			int* intData = new int[dataSize / sizeof(int)];
			samplerFile.read(reinterpret_cast<char*>(intData), dataSize);
			data = new float[numSamples];

			for (int i = 0; i < numSamples; ++i) {

				data[i] = *intData / MAX_32;
				++intData;
			}

			intData -= numSamples;
			delete[] intData;
		}
		else {

			std::stringstream stream;
			stream << bitDepth;

			// if this gets thrown ask Morgen to implement your favorite bit depth!
			std::cout << std::string("Failed to read file " + samplePath + ", unsupported bit depth: " + stream.str()).c_str() << std::endl;
		}

		m_audioFileData = data;
		m_fileSampleCount = numSamples;
	}

	void PianoSynth::generateAndAdd(float* output, uint8_t pitch, uint32_t numSamples, float gain) {

		// Determine frequency of root note and generate
		double exponent = static_cast<double>(pitch - MIDI_A440) / static_cast<double>(OctaveInterval);
		float frequency = 440.0f * static_cast<float>(std::pow(2.0, exponent));

		// If notes are outside range of the sample lib don't play anything
		if (pitch < 21 | pitch > 108) { return; }

		m_sampleTable[pitch - 21].generateSamples(m_tempBuffer, numSamples);

		// Add root to output buffer
		for (uint32_t sample = 0; sample < numSamples; ++sample) {

			output[sample] += (m_tempBuffer[sample] * gain);
		}

	}

	PianoSynth::PianoSample::PianoSample(float* samples, uint32_t sampleLength, const Envelope& envelopeSource)
		: m_samples(samples), m_arrLen(sampleLength), m_envSource(envelopeSource) {
	}


	void PianoSynth::PianoSample::generateSamples(float* samples, uint32_t numSamples) {
		
		// Boundry checking on debug mode for note length relative to sample length
#ifdef _DEBUG
		if (numSamples > m_arrLen) {

			std::cout << "Error, piano synthesizer encountered a note request too long" << std::endl;
			std::memset(samples, 0, numSamples * sizeof(float));
			return;
		}
#endif

		// Memcpy the samples directly into the output
		std::memcpy(samples, m_samples, numSamples * sizeof(float));

		// Apply gain envelope to the note to get correct attack and release sound
		m_envSource.ApplyGain(samples, numSamples);
	}


} // namespace Genetics