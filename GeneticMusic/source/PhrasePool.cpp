/*
** Author  - Morgen Hyde
** Project - MusicGenetics
** Date    - 10 / 21 / 2019
*/

#include "PhrasePool.h"
#include "GADefaultConfig.h"

#include <iostream>  // std::cout
#include <algorithm> // std::sort

namespace Genetics {

	unsigned Phrase::_phraseCount = 0;

	unsigned Phrase::_numMeasures         = 0;
	unsigned Phrase::_smallestSubdivision = 0;

	PhrasePool::PhrasePool(PoolAllocator<Phrase>* poolAlloc, unsigned measureCount, unsigned subDivision)
		: m_poolAllocator(poolAlloc), m_measureCount(measureCount), m_subDivision(subDivision) {

		m_population.reserve(poolAlloc->capacity() - 1);
	}

	PhrasePool::~PhrasePool() {

		for (Phrase* iter : m_population) {
			m_poolAllocator->free(iter);
		}
		for (Phrase* iter : m_childPopulation) {
			m_poolAllocator->free(iter);
		}
	}

	Phrase* PhrasePool::AllocateChild() {

		Phrase* newPhrase = m_poolAllocator->alloc();
		if (newPhrase) {

			// Allocate space for a note every 16th to make later operations easier
			unsigned int maxNotes = m_measureCount * m_subDivision;

			newPhrase->_melodicNotes  = 0;
			newPhrase->_melodicData   = new char[maxNotes];
			newPhrase->_melodicRhythm = new char[maxNotes];

			newPhrase->_harmonicNotes = 0;
			newPhrase->_harmonicData  = new Chord[m_measureCount * 4];

			// Set all arrays to 0
			std::memset(newPhrase->_melodicData,   0, maxNotes);
			std::memset(newPhrase->_melodicRhythm, 0, maxNotes);

			std::memset(newPhrase->_harmonicData,  0, m_measureCount * 4 * sizeof(Chord));

			m_childPopulation.push_back(newPhrase);
		}
		return newPhrase;
	}

	unsigned PhrasePool::GetPhraseNumberOf(Phrase* phrase) const {

		unsigned phraseNum = 1;
		for (Phrase* iter : m_population)
		{
			if (iter == phrase) {
				return phraseNum;
			}

			++phraseNum;
		}

		return 0;
	}

	void PhrasePool::DisplayRatings() const {

		for (int i = 0; i < m_population.size(); ++i)
		{
			std::cout << "Measure #" << i + 1 << " has rating: ";
			std::cout << m_population[i]->_fitnessValue << std::endl;
		}
	}

	// Elitist Uses the best parents and their children first
	void ElitistPrune::Merge(PoolAllocator<Phrase>* poolAlloc, PhraseVec& parents, PhraseVec& children) {

		// Store the population size
		uint32_t populationSize = static_cast<uint32_t>(parents.size());

		// Add all the children to the parent vector and sort it in order of decreasing fitness values
		parents.insert(parents.end(), children.begin(), children.end());
		std::sort(parents.begin(), parents.end(), PhraseFitnessSorter());

		children.clear();

		// Remove elements till we're back at the correct size
		while (parents.size() > populationSize) {

			poolAlloc->free(parents.back());
			parents.pop_back();
		}

	}

	// Generational is a complete replacement of parents with children
	void GenerationalPrune::Merge(PoolAllocator<Phrase>* poolAlloc, PhraseVec& parents, PhraseVec& children) {

		// Free all the parents in the parent vector
		for (Phrase* parent : parents) {

			poolAlloc->free(parent);
		}

		// Clear the vector
		parents.clear();

		// Take each child and add to the parent vector
		for (Phrase* child : children) {

			parents.push_back(child);
		}

		// Clear the child vector
		children.clear();
	}

	// Truncation just uses the best N members of the population regardless of child or parent status
	void TruncationPrune::Merge(PoolAllocator<Phrase>* poolAlloc, PhraseVec& parents, PhraseVec& children) {


	}

} // namespace Genetics