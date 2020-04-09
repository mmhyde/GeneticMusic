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
		RouletteSelection();
		~RouletteSelection();

	protected:
		BreedingPair Select(PhrasePool* phrasePopulation);

	private:
		std::mt19937 m_randomEngine;

	};

	struct TournamentSelection {

	public:
		TournamentSelection();
		~TournamentSelection();

		void SetNumRounds(unsigned numRounds = 1);
		void SetParentPoolSize(unsigned numPossibleParents = 2);

	protected:
		BreedingPair Select(PhrasePool* phrasePopulation);
	
	private:
		Phrase* RunTournament(const std::vector<Phrase*>& population, int parents);

		unsigned m_numRounds;
		unsigned m_possibleParents;

		std::mt19937 m_randomEngine;
	};


	template <class SelectorPolicy = RouletteSelection>
	class Selection : public SelectorPolicy
	{
	public:
		Selection();
		~Selection();

		BreedingPair SelectPair(PhrasePool* phrasePopulation);
	};

	template <class Policy>
	Selection<Policy>::Selection() {
	}

	template <class Policy>
	Selection<Policy>::~Selection() {
	}

	template <class Policy>
	BreedingPair Selection<Policy>::SelectPair(PhrasePool* phrasePopulation)
	{
		return Policy::Select(phrasePopulation);
	}

} // namespace Genetic