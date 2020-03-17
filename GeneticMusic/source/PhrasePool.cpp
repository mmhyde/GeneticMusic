/*
** Author  - Morgen Hyde
** Project - MusicGenetics
** Date    - 10 / 21 / 2019
*/

#include "PhrasePool.h"
#include <iostream>

namespace Genetics {

	unsigned Phrase::_phraseCount = 0;

	unsigned Phrase::_numMeasures         = 0;
	unsigned Phrase::_smallestSubdivision = 0;

	PhrasePool::PhrasePool(PoolAllocator<Phrase>& poolAlloc, unsigned measureCount, unsigned subDivision)
		: m_poolAllocator(poolAlloc), m_measureCount(measureCount), m_subDivision(subDivision) {

		m_population.reserve(poolAlloc.Capacity() - 1);
	}

	PhrasePool::~PhrasePool() {

		for (Phrase* iter : m_population) {
			m_poolAllocator.Free(iter);
		}
		for (Phrase* iter : m_childPopulation) {
			m_poolAllocator.Free(iter);
		}
	}

	Phrase* PhrasePool::AllocateChild() {

		Phrase* newPhrase = m_poolAllocator.Alloc();
		if (newPhrase) {

			// Allocate space for a note every 16th to make later operations easier
			unsigned int maxNotes = m_measureCount * m_subDivision;

			newPhrase->_melodicNotes  = 0;
			newPhrase->_melodicData   = new char[maxNotes];
			newPhrase->_melodicRhythm = new char[maxNotes];
			/*
			newPhrase->_chordCount  = 0;
			newPhrase->_chordData   = new Triad[maxNotes];
			newPhrase->_chordRhythm = new Triad[maxNotes];
			*/
			// Set all arrays to 0
			std::memset(newPhrase->_melodicData,   0, maxNotes);
			std::memset(newPhrase->_melodicRhythm, 0, maxNotes);
			/*
			std::memset(newPhrase->_chordData,   0, maxNotes * sizeof(Triad));
			std::memset(newPhrase->_chordRhythm, 0, maxNotes * sizeof(Triad));
			*/
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
	void ElitistPrune::Merge(PoolAllocator<Phrase>& poolAlloc, PhraseVec& parents, PhraseVec& children) {


	}

	// Generational is a complete replacement of parents with children
	void GenerationalPrune::Merge(PoolAllocator<Phrase>& poolAlloc, PhraseVec& parents, PhraseVec& children) {

		// Free all the parents in the parent vector
		for (Phrase* parent : parents) {

			poolAlloc.Free(parent);
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
	void TruncationPrune::Merge(PoolAllocator<Phrase>& poolAlloc, PhraseVec& parents, PhraseVec& children) {


	}

} // namespace Genetics