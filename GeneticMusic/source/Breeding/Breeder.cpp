
#include "Breeding/Breeder.h"
#include "Phrase.h"

#include <iostream>

namespace Genetics {

	Breeder::Breeder()
		: m_tempBuffer(nullptr), m_tempBufferSize(0)
	{
		std::random_device rd;
		m_randomEngine.seed(rd());
	}

	Breeder::~Breeder()
	{

	}

	void Breeder::Breed(const BreedingPair& phraseParents, unsigned measureCount, unsigned subdivision)
	{
		Phrase* parentA = phraseParents.first;
		Phrase* parentB = phraseParents.second;

		unsigned measureHalfWidth = (subdivision / 2);
		if (measureHalfWidth > m_tempBufferSize) {

			if (m_tempBuffer) { delete[] m_tempBuffer; }
			m_tempBuffer = new char[measureHalfWidth];
			m_tempBufferSize = measureHalfWidth;
		}
		
		for (unsigned measure = 0; measure < measureCount; ++measure) {

			int measureIndex = measure * subdivision;

			// If either of the two measures contain a wholenote, we can't perform a cross point
			// breeding operation and need to do something special
			if (parentA->_melodicRhythm[measureIndex] == subdivision ||
				parentB->_melodicRhythm[measureIndex] == subdivision) {

				// The special thing we do is to do nothing
				continue;
			}

			// No weird edge cases we do a basic crossover breeding operation
			// memcpy the second half of parentA's measure into parentB and vice versa

			// Copy melodic information
			char* parentAData = parentA->_melodicData + measureIndex;
			char* parentBData = parentB->_melodicData + measureIndex;

			// Determine number of notes in the two halves
			unsigned parentAHalfNotes = 0;
			unsigned parentBHalfNotes = 0;
			for (unsigned i = 0; i < measureHalfWidth; ++i) {
				if (parentAData[i] != 0) {
					++parentAHalfNotes;
				}
				if (parentBData[i] != 0) {
					++parentBHalfNotes;
				}
			}

			parentA->_melodicNotes -= parentAHalfNotes;
			parentA->_melodicNotes += parentBHalfNotes;

			parentB->_melodicNotes -= parentBHalfNotes;
			parentB->_melodicNotes += parentAHalfNotes;

			std::memcpy(m_tempBuffer, parentAData,  measureHalfWidth);
			std::memcpy(parentAData,  parentBData,  measureHalfWidth);
			std::memcpy(parentBData,  m_tempBuffer, measureHalfWidth);

			// Copy rhythmic information
			parentAData = parentA->_melodicRhythm + measureIndex;
			parentBData = parentB->_melodicRhythm + measureIndex;

			std::memcpy(m_tempBuffer, parentAData,  measureHalfWidth);
			std::memcpy(parentAData,  parentBData,  measureHalfWidth);
			std::memcpy(parentBData,  m_tempBuffer, measureHalfWidth);
		}
	}


	void CrosspointBreed::CreateChildren(const BreedingPair& parents, PhrasePool* phrasePool) {

		Phrase* parentA = parents.first;
		Phrase* parentB = parents.second;

		unsigned subdivision = parentA->_smallestSubdivision;
		unsigned measureCount = parentA->_numMeasures;

		unsigned measureHalfWidth = (subdivision / 2);
		if (measureHalfWidth > m_tempBufferSize) {

			if (m_tempBuffer) { delete[] m_tempBuffer; }
			m_tempBuffer = new char[measureHalfWidth];
			m_tempBufferSize = measureHalfWidth;
		}

		Phrase* child = phrasePool->AllocateChild();

		for (unsigned measure = 0; measure < measureCount; ++measure) {

			int measureIndex = measure * subdivision;

			// If either of the two measures contain a wholenote, we can't perform a cross point
			// breeding operation and need to do something special
			if (parentA->_melodicRhythm[measureIndex] == subdivision ||
				parentB->_melodicRhythm[measureIndex] == subdivision) {

				// The special thing we do is to do nothing
				continue;
			}

			// No weird edge cases we do a basic crossover breeding operation
			// memcpy the second half of parentA's measure into parentB and vice versa

			// Copy melodic information
			char* parentAData = parentA->_melodicData + measureIndex;
			char* parentBData = parentB->_melodicData + measureIndex;

			// Determine number of notes in the two halves
			unsigned parentAHalfNotes = 0;
			unsigned parentBHalfNotes = 0;
			for (unsigned i = 0; i < measureHalfWidth; ++i) {
				if (parentAData[i] != 0) {
					++parentAHalfNotes;
				}
				if (parentBData[i] != 0) {
					++parentBHalfNotes;
				}
			}

			parentA->_melodicNotes -= parentAHalfNotes;
			parentA->_melodicNotes += parentBHalfNotes;

			parentB->_melodicNotes -= parentBHalfNotes;
			parentB->_melodicNotes += parentAHalfNotes;

			std::memcpy(m_tempBuffer, parentAData, measureHalfWidth);
			std::memcpy(parentAData, parentBData, measureHalfWidth);
			std::memcpy(parentBData, m_tempBuffer, measureHalfWidth);

			// Copy rhythmic information
			parentAData = parentA->_melodicRhythm + measureIndex;
			parentBData = parentB->_melodicRhythm + measureIndex;

			std::memcpy(m_tempBuffer, parentAData, measureHalfWidth);
			std::memcpy(parentAData, parentBData, measureHalfWidth);
			std::memcpy(parentBData, m_tempBuffer, measureHalfWidth);
		}
	}

	__inline unsigned GetPowerOfTwo(unsigned value) {

		unsigned counter = 0;
		while (value > 1) {

			counter++;
			value = value >> 1;
		}
		return counter;
	}

	__inline int PitchInterpolate(int low, int high, float alpha) {

		return static_cast<int>(low + alpha * (high - low));
	}

	__inline int RhythmicInterpolate(int low, int high, float alpha) {
		
		// Ensure we get the positive range of the two numbers (negative gets weird) 
		int bins = ((high - low < 0) ? (low - high) : (high - low)) + 1;
		float binWeight = 1.0f / static_cast<float>(bins);
		int output = std::min(low, high);
		for (; output < bins; ++output) {
			
			alpha -= binWeight;
			if (alpha <= 0.0f) {
				return output;
			}
		}

		return output;
	}

	void InterpolateBreed::CreateChildren(const BreedingPair& parents, PhrasePool* phrasePool) {

		// Step 1, Find the percentage of contribution from each parent using their respective weights
		// Formula for ratio: f_1 / (f_1 + f_2)
		Phrase* parent1 = parents.first;
		Phrase* parent2 = parents.second;
		Phrase* child   = phrasePool->AllocateChild();

		float fitness1 = parent1->_fitnessValue; // 0.0
		float fitness2 = parent2->_fitnessValue; // 1.0

		float interpolationRatio = fitness2 / (fitness1 + fitness2);

		// Step 2, Loop over the phrases
		unsigned subDiv = parent1->_smallestSubdivision;
		unsigned measureCount = parent1->_numMeasures;
		unsigned maxNotes = subDiv * measureCount;

		unsigned note1 = 0, note2 = 0, output = 0;

		while (output < maxNotes) {

			// Step 3, Start with rhythm interpolation.
			// interpolate to nearest acceptable rhythmic value based on the ratio

			// At the start of every outer loop note1, note2, and output should all be equal
			int note1Val = GetPowerOfTwo(parent1->_melodicRhythm[note1]);
			int note2Val = GetPowerOfTwo(parent2->_melodicRhythm[note2]);
			
			int remaining = std::max(1 << note1Val, 1 << note2Val);
			int outputVal = 0;
			while (remaining > 0) {

				outputVal = RhythmicInterpolate(note1Val, note2Val, interpolationRatio);
				outputVal = 1 << outputVal;

				remaining -= outputVal;
				child->_melodicRhythm[output] = static_cast<char>(outputVal);

				int note1Pitch = parent1->_melodicData[note1];
				int note2Pitch = parent2->_melodicData[note2];

				int newPitch = PitchInterpolate(note1Pitch, note2Pitch, interpolationRatio);
				
				if (note1Pitch == 0) {
					int avg = (note1Pitch + note2Pitch) / 2;
					newPitch = (newPitch < avg) ? note1Pitch : note2Pitch;
				}
				else if (note2Pitch == 0) {
					int avg = (note1Pitch + note2Pitch) / 2;
					newPitch = (newPitch > avg) ? note1Pitch : note2Pitch;
				}

				child->_melodicData[output] = newPitch;

				output += outputVal;
				child->_melodicNotes += 1;

				// Traverse the array of the smaller rhythmic value 
				// In the event the two are equal remaining should be 0 in 
				// which case we need to move both anyway
				if (remaining > 0 && note1Val < note2Val) {

					note1 += 1 << note1Val;
					note1Val = GetPowerOfTwo(parent1->_melodicRhythm[note1]);
				}
				else if (remaining > 0 && note2Val < note1Val) {

					note2 += 1 << note2Val;
					note2Val = GetPowerOfTwo(parent2->_melodicRhythm[note2]);
				}
			}

			if (remaining < 0) {

				// Readjust the final rhythmic value to be syntactically correct
				// in line with your binary rhythm model
				output -= outputVal;
				child->_melodicRhythm[output] += remaining;
				output += child->_melodicRhythm[output];
			}

			// Adjust note_x variables to be at the start of the next segment
			if (note1Val > note2Val) {

				note1 += 1 << note1Val;
				note2 = note1;
			}
			else {

				note2 += 1 << note2Val;
				note1 = note2;
			}
		}

		//std::cout << "Num Child Notes: " << child->_melodicNotes << std::endl;
	}

} // namespace Genetics