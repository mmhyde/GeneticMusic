#pragma once

#include <utility>
#include <random>

namespace Genetics {

	class PhrasePool;
	struct Phrase;

	typedef std::pair<Phrase*, Phrase*> BreedingPair;

	struct RouletteSelection
	{
	public:
		// Intentionally Blank

	protected:
		BreedingPair Select(PhrasePool* phrasePopulation, std::mt19937& randomEngine);

		~RouletteSelection() {}
	};

	struct TournamentSelection {

	public:
		void SetNumRounds(unsigned numRounds = 1);
		void SetParentPoolSize(unsigned numPossibleParents = 2);

	protected:
		BreedingPair Select(PhrasePool*, std::mt19937&);

		~TournamentSelection() {}
	
	private:
		unsigned m_numRounds;
		unsigned m_possibleParents;

	};


	template <class SelectorPolicy = RouletteSelection>
	class Selection : public SelectorPolicy
	{
	public:
		Selection();
		~Selection();

		BreedingPair SelectPair(PhrasePool* phrasePopulation);

	private:
		std::mt19937 m_randomEngine;
	};

	template <class Policy>
	Selection<Policy>::Selection()
	{
		std::random_device rd;
		m_randomEngine.seed(rd());
	}

	template <class Policy>
	Selection<Policy>::~Selection()
	{
	}

	template <class Policy>
	BreedingPair Selection<Policy>::SelectPair(PhrasePool* phrasePopulation)
	{
		return this->Select(phrasePopulation, m_randomEngine);
	}

} // namespace Genetic