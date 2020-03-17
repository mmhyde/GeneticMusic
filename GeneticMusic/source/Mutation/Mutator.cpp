
#include "Mutation/Mutator.h"
#include "Phrase.h"

#include <iostream>

namespace Genetics {




#define ADD_MUTATION(weight, mutation) \
	m_mutationWeights.push_back(weight); \
	m_mutationPool.push_back(&Mutator::mutation)

//#define DEBUG_OUTPUT 1


	Mutator::Mutator()
	{
		std::random_device rd;
		m_randomEngine.seed(rd());
	}

	Mutator::~Mutator()
	{

	}

	void Mutator::InitMutationPool()
	{
		//ADD_MUTATION(30, NullOperator);
		ADD_MUTATION(20, Subdivide);
		ADD_MUTATION(20, Merge);
		//ADD_MUTATION(10, Rest);
		//ADD_MUTATION(10, NullOperator);

		m_numMutations = static_cast<unsigned>(m_mutationWeights.size());
	}

	void Mutator::Mutate(Phrase* phrase) {

		std::uniform_int_distribution<int> distrib(0, 40);
		short choice = distrib(m_randomEngine);

		unsigned index = 0;
		for (; index < m_numMutations; ++index) {
			choice -= m_mutationWeights[index];
			if (choice <= 0) {
				break;
			}
		}

		(this->*(m_mutationPool[index]))(phrase);
	}


	void Mutator::NullOperator(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
		std::cout << "Picked the null operator mutation" << std::endl;
#endif
		// Intentionally Blank
	}

	void Mutator::Subdivide(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
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

#ifdef DEBUG_OUTPUT
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
			
			// Step 1: Determine if we can merge this note
			// We CAN if:
			// - The note isn't a whole note (duh) 
			// - The note is on a "strong" beat relative to it's size (math)
			if (noteLength != subdivision && (normalizedIndex % (noteLength << 1) == 0)) {

				// Probability!
				for (unsigned shifted = noteLength; shifted > 1; shifted >>= 1) {

					mergeProbability *= 0.5f;
				}

				// Setup and perform the probability step to determine if we merge or not
				float probability = mergeProbability * currentProbabilityMod;
				std::uniform_int_distribution<int> distribution(1, static_cast<int>(MaxRollNum));
				int roll = distribution(m_randomEngine);

				if ((static_cast<float>(roll) / MaxRollNum) <= probability) {

					/*
					std::cout << phrase->_phraseID << std::endl;
					std::cout << "Before merge happens, numNotes = " << phrase->_melodicNotes << std::endl;

					for (unsigned i = 0; i < maxNotes; ++i) {
						std::cout << static_cast<int>(phrase->_melodicRhythm[i]) << " ";
					}

					std::cout << std::endl;
					*/

					// Remove the merged note from the array
					phrase->_melodicRhythm[note + noteLength] = 0;
					phrase->_melodicData[note + noteLength] = 0;

					// Change the size of the current note
					phrase->_melodicRhythm[note] = (noteLength << 1);
					phrase->_melodicNotes -= 1; // decrement note count

					/*
					std::cout << "After merge takes place, numNotes = " << phrase->_melodicNotes << std::endl;

					for (unsigned i = 0; i < maxNotes; ++i) {
						std::cout << static_cast<int>(phrase->_melodicRhythm[i]) << " ";
					}

					std::cout << std::endl;
					std::cout << std::endl;
					*/

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

#ifdef DEBUG_OUTPUT
		std::cout << "Picked Rest mutation" << std::endl;
#endif
	}

	// Pitch based mutation operations

	void Mutator::Rotate(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
		std::cout << "Picked rotate mutation" << std::endl;
#endif


	}

	void Mutator::Transpose(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
		std::cout << "Picked transpose mutation" << std::endl;
#endif
	}

	void Mutator::SortAscending(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
		std::cout << "Picked sort (ascending) mutation" << std::endl;
#endif
	}

	void Mutator::SortDescending(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
		std::cout << "Picked sort (descending) mutation" << std::endl;
#endif
	}


	void Mutator::Inversion(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
		std::cout << "Picked Inversion mutation" << std::endl;
#endif
	}

	void Mutator::Retrograde(Phrase* phrase) {

#ifdef DEBUG_OUTPUT
		std::cout << "Picked Retrograde mutation" << std::endl;
#endif
	}

} // namespace Genetics