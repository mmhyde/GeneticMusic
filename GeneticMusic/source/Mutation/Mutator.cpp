// Morgen Hyde

#include "Mutation/Mutator.h"
#include "Phrase.h"
#include "GADefaultConfig.h"

#include <iostream>
#include <queue>

namespace Genetics {

#define ADD_MUTATION(weight, mutation) \
	m_mutationWeights.push_back(weight); \
	m_mutationPool.push_back(&Mutator::mutation)

	Mutator::Mutator()
	{
		std::random_device rd;
		m_randomEngine.seed(rd());
	}

	Mutator::~Mutator() {

	}

	void Mutator::InitMutationPool()
	{
		// No Op
		ADD_MUTATION(30, NullOperator);

		// Rhythm mutators
		ADD_MUTATION(15, Subdivide);
		ADD_MUTATION(15, Merge);

		// Pitch Mutators
		ADD_MUTATION(20, Rotate);
		ADD_MUTATION(20, Transpose);
		ADD_MUTATION(20, SortAscending);
		ADD_MUTATION(20, SortDescending);
		ADD_MUTATION(20, Inversion);
		ADD_MUTATION(20, Retrograde);

		m_numMutations = static_cast<unsigned>(m_mutationWeights.size());
	}

	void Mutator::Mutate(Phrase* phrase) {

		int weightSum = 0;
		for (int weight : m_mutationWeights) {
			weightSum += weight;
		}

		std::uniform_int_distribution<int> distrib(0, weightSum);
		short choice = distrib(m_randomEngine);

		//for (uint16_t measure = 0; measure < Phrase::_numMeasures; ++measure) {

			unsigned index = 0;
			for (; index < m_numMutations; ++index) {
				choice -= m_mutationWeights[index];
				if (choice <= 0) {
					break;
				}
			}

			(this->*(m_mutationPool[index]))(phrase);
		//}


	}


	void Mutator::NullOperator(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked the null operator mutation" << std::endl;
#endif
		// Intentionally Blank
	}

	void Mutator::Subdivide(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked Subdivision Mutation" << std::endl;
#endif

		// We want to weight the probabilities of subdividing each note based on how long it is
		// We also want the probability to increase slightly for each note that we 
		// don't decide to subdivide

		// Starting probability of .4f for a whole note
		// divide by half as we go down in size
		// e.g. : 0.2 for half, 0.1 for quarter, 0.05 for eighth, 0.025 for sixteenth
		const float StartingProbability = 0.4f;
		const float nochangeModifier    = 1.25f;
		const float MaxRollNum          = 1000.0f;

		float currentProbabilityMod = 1.0f;

		unsigned measureCount = phrase->_numMeasures;
		unsigned subdivision  = phrase->_smallestSubdivision;

		unsigned maxNotes = measureCount * subdivision;
		for (unsigned note = 0; note < maxNotes;) {

			// Set the baseline odds for a subdivision to occur on this note
			float subDivideProbability = StartingProbability;

			int currentNoteValue = phrase->_melodicRhythm[note];

			// Check that the current value can be subdivided, ignore this index if it can't
			if (currentNoteValue == 1) {

				++note;
				continue;
			}

			// For as many times as we can divide by 2 before we get smaller than 16th note,
			// divide the odds by 2
			for (unsigned shifted = currentNoteValue; shifted != subdivision; shifted <<= 1) {

				subDivideProbability *= 0.5f;
			}

			float probability = subDivideProbability * currentProbabilityMod;
			std::uniform_int_distribution<int> distribution(1, static_cast<int>(MaxRollNum));
			int roll = distribution(m_randomEngine);

			// If this passes we subdivide whatever the current value is
			if ((static_cast<float>(roll) / MaxRollNum) <= probability) {

				/*
				std::cout << phrase->_phraseID << std::endl;
				std::cout << "Before split happens, numNotes = " << phrase->_melodicNotes << std::endl;

				for (unsigned i = 0; i < maxNotes; ++i) {
					std::cout << static_cast<int>(phrase->_melodicRhythm[i]) << " ";
				}

				std::cout << std::endl;
				*/

				// Insert half the note length at the original position
				currentNoteValue = currentNoteValue / 2;

				phrase->_melodicRhythm[note] = currentNoteValue;
				int pitchValue = phrase->_melodicData[note];

				// Move the index along to the new note
				note += currentNoteValue;

				// Insert the new note in the phrase
				phrase->_melodicRhythm[note] = currentNoteValue;
				phrase->_melodicData[note] = pitchValue;

				phrase->_melodicNotes += 1;

				/*
				std::cout << "After split happens, numNotes = " << phrase->_melodicNotes << std::endl;

				for (unsigned i = 0; i < maxNotes; ++i) {
					std::cout << static_cast<int>(phrase->_melodicRhythm[i]) << " ";
				}

				std::cout << std::endl;
				std::cout << std::endl;
				*/

				currentProbabilityMod = 1.0f;
			}
			else
			{
				currentProbabilityMod *= nochangeModifier;
			}
			
			note += currentNoteValue;
		}
	}

	void Mutator::Merge(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked Merge mutation" << std::endl;
#endif

		// Same model for merging as we do with subdividing
		// Base probability starts at .4f for smallest merge
		// goes down by half for each larger size
		
		// Extra Complication! We have to keep things along a binary boundry
		// No weird off beats!

		const float StartingProbability = 0.4f;
		const float nochangeModifier = 1.25f;
		const float MaxRollNum = 1000.0f;

		float currentProbabilityMod = 1.0f;
		unsigned normalizedIndex = 0;

		unsigned measureCount = phrase->_numMeasures;
		unsigned subdivision = phrase->_smallestSubdivision;

		unsigned maxNotes = measureCount * subdivision;
		for (unsigned note = 0; note < maxNotes;) {

			float mergeProbability = StartingProbability;
			int noteLength = phrase->_melodicRhythm[note];
			int mergeTarget = phrase->_melodicRhythm[note + noteLength];
			
			// Step 1: Determine if we can merge this note
			// We CAN if:
			// - The note isn't a whole note
			// - The note is on a "strong" beat relative to it's size
			// - The note and the merge target are the same length (Can't merge a quarter and an eighth for example)
			if (noteLength != subdivision && (normalizedIndex % (noteLength << 1) == 0) && noteLength == mergeTarget) {

				// Probability!
				for (unsigned shifted = noteLength; shifted > 1; shifted >>= 1) {

					mergeProbability *= 0.5f;
				}

				// Setup and perform the probability step to determine if we merge or not
				float probability = mergeProbability * currentProbabilityMod;
				std::uniform_int_distribution<int> distribution(1, static_cast<int>(MaxRollNum));
				int roll = distribution(m_randomEngine);

				if ((static_cast<float>(roll) / MaxRollNum) <= probability) {

					// Remove the merged note from the array
					phrase->_melodicRhythm[note + noteLength] = 0;
					phrase->_melodicData[note + noteLength] = 0;

					// Change the size of the current note
					phrase->_melodicRhythm[note] = (noteLength << 1);
					phrase->_melodicNotes -= 1; // decrement note count

					if (phrase->_melodicNotes < 2) {
						std::cout << "Something very bad happened" << std::endl;

						for (unsigned i = 0; i < maxNotes; ++i) {
							std::cout << static_cast<int>(phrase->_melodicRhythm[i]) << " ";
						}
						
						std::cout << std::endl;
						
					}

					// Reset probability modifier
					currentProbabilityMod = 1.0f;

					// Update note length
					noteLength = noteLength << 1;
				}
				else
				{
					// Increase the modifier since we didn't do nuthin'
					currentProbabilityMod *= nochangeModifier;
				}
			} 

			note += noteLength;
			normalizedIndex += noteLength;
			normalizedIndex = normalizedIndex & 0x0F; // This isn't guaranteed 
		}
	}

	void Mutator::Rest(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked Rest mutation" << std::endl;
#endif
	}

	// Pitch based mutation operations //

	// Shifts all the pitches in the melody n notes to the left or right
	void Mutator::Rotate(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked rotate mutation" << std::endl;
#endif
		constexpr int Left = 1;
		constexpr int Right = 2;

		// Make a distribution to pick a left or right rotation of notes
		//std::uniform_int_distribution<int> rotateDir(Left, Right);
		//int direction = rotateDir(m_randomEngine);

		// Make another distribution to pick how far to rotate each note
		std::uniform_int_distribution<int> rotationAmount(1, phrase->_melodicNotes - 1);
		int rotate = rotationAmount(m_randomEngine);

		// Make a queue to store values in temporarily as we write
		std::queue<char> rotatedPitches;

		// Fill queue with first elements
		int noteIndex = 0;
		for (int i = 0; i < rotate; ++i) {

			rotatedPitches.push(phrase->_melodicData[noteIndex]);
			noteIndex += phrase->_melodicRhythm[noteIndex];
		}

		int arrayLen = static_cast<int>(phrase->_numMeasures * phrase->_smallestSubdivision);

		// Loop over the entire length of the array performing the rotation
		for (int i = 0; i < static_cast<int>(phrase->_melodicNotes); ++i) {

			// Use modulo operator to ensure valid range and correct wrapping
			noteIndex = noteIndex % arrayLen;

			// Store old value for later
			rotatedPitches.push(phrase->_melodicData[noteIndex]);

			// Grab next element from the queue
			phrase->_melodicData[noteIndex] = rotatedPitches.front();
			rotatedPitches.pop();

			// Update index off the rhythm value
			noteIndex += phrase->_melodicRhythm[noteIndex];
		}
	}

	void Mutator::Transpose(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked transpose mutation" << std::endl;
#endif

		// Create a distribution for the number of semitones to shift
		std::uniform_int_distribution<int> semitoneShift(-12, 12);
		
		char shiftAmount = static_cast<char>(semitoneShift(m_randomEngine));

		int noteIndex = 0;
		for (int i = 0; i < static_cast<int>(phrase->_melodicNotes); ++i) {

			// Shift each pitch by the shift amount
			phrase->_melodicData[noteIndex] += shiftAmount;

			char pitchVal = phrase->_melodicData[noteIndex];

			// Reflect pitches to keep them in the correct range
			if (pitchVal > MaxPitch) {
				phrase->_melodicData[noteIndex] = MaxPitch - (pitchVal - MaxPitch);
			}
			else if (pitchVal < MinPitch) {
				phrase->_melodicData[noteIndex] = MinPitch + (MinPitch - pitchVal);
			}

			// Update index of rhythmic value
			noteIndex += phrase->_melodicRhythm[noteIndex];
		}
	}

	void Mutator::SortAscending(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked sort (ascending) mutation" << std::endl;
#endif

		std::vector<char> pitches;
		uint32_t arrayLen = Phrase::_numMeasures * Phrase::_smallestSubdivision;
		pitches.reserve(arrayLen);

		// Build a vector of all pitches in the melody
		uint32_t note = 0;
		for (uint32_t i = 0; i < phrase->_melodicNotes; ++i) {

			pitches.push_back(phrase->_melodicData[note]);
			note += phrase->_melodicRhythm[note];
		}

		// Sort lowest to highest
		std::sort(pitches.begin(), pitches.end());

		// Place notes back in the melody in ascending order
		
		for (uint32_t i = 0, note = 0; i < phrase->_melodicNotes; ++i) {

			phrase->_melodicData[note] = pitches[i];
			note += phrase->_melodicRhythm[note];
		}
	}

	void Mutator::SortDescending(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked sort (descending) mutation" << std::endl;
#endif

		std::vector<char> pitches;
		pitches.reserve(phrase->_melodicNotes);

		// Build a vector of all pitches in the melody
		uint32_t note = 0;
		for (uint32_t i = 0; i < phrase->_melodicNotes; ++i) {

			pitches.push_back(phrase->_melodicData[note]);
			note += phrase->_melodicRhythm[note];
		}

		// Sort highest to lowest
		std::sort(pitches.begin(), pitches.end(), std::greater<char>());

		// Place notes back in the melody in ascending order
		
		for (uint32_t i = 0, note = 0; i < phrase->_melodicNotes; ++i) {

			phrase->_melodicData[note] = pitches[i];
			note += phrase->_melodicRhythm[note];
		}
	}


	void Mutator::Inversion(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked Inversion mutation" << std::endl;
#endif

		std::vector<char> pitches;
		pitches.reserve(phrase->_melodicNotes);

		// Build a vector of all pitches in the melody
		uint32_t note = 0;
		for (uint32_t i = 0; i < phrase->_melodicNotes; ++i) {

			pitches.push_back(phrase->_melodicData[note]);
			note += phrase->_melodicRhythm[note];
		}

		// Sort to find midpoint
		std::sort(pitches.begin(), pitches.end());
		uint32_t midpoint = static_cast<uint32_t>(pitches.size() / 2);

		// Find the center pitch to flip notes around
		char centerPitch = phrase->_melodicData[midpoint];
		
		// If we have an even number of notes, average the middle two values
		if (pitches.size() % 2 == 0) {
			centerPitch += phrase->_melodicData[midpoint - 1];
			centerPitch /= 2;
		}

		// Loop over each pitch
		for (uint32_t i = 0, note = 0; i < phrase->_melodicNotes; ++i) {

			// Determine shift amount of each pitch, and perform the shift
			char shiftAmount = 2 * (centerPitch - phrase->_melodicData[note]);
			phrase->_melodicData[note] += shiftAmount;

			note += phrase->_melodicRhythm[note];
		}
	}

	void Mutator::Retrograde(Phrase* phrase) {

#ifdef _DEBUG
		std::cout << "Picked Retrograde mutation" << std::endl;
#endif

		std::vector<char> pitches;
		pitches.reserve(phrase->_melodicNotes);

		// Build vector of pitches to reverse the order later
		uint32_t note = 0;
		for (uint32_t i = 0; i < phrase->_melodicNotes; ++i) {

			pitches.push_back(phrase->_melodicData[note]);
			note += phrase->_melodicRhythm[note];
		}

		// Place the notes back in reverse order
		for (uint32_t i = 0, note = 0; i < phrase->_melodicNotes; ++i) {

			phrase->_melodicData[note] = pitches.back();
			pitches.pop_back();
			note += phrase->_melodicRhythm[note];
		}
	}

} // namespace Genetics