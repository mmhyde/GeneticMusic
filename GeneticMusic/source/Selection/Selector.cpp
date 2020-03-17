// Morgen Hyde

#include <iostream>

#include "Selection/Selector.h"

#include "PhrasePool.h"
#include "Phrase.h"

namespace Genetics {

	BreedingPair RouletteSelection::Select(PhrasePool* phrasePopulation, std::mt19937& randomEngine) {

		const std::vector<Phrase*>& phrases = phrasePopulation->GetPhrases();
		float totalFitness = 0.0f;

		Phrase* selection1 = nullptr;
		Phrase* selection2 = nullptr;

		// Add Up the total fitness of the phrase pool
		for (Phrase* iter : phrases) {
			totalFitness += iter->_fitnessValue;
		}

		// Generate a number from 0 to 100 and divide by total fitness
		std::uniform_int_distribution<int> distrib(0, 99);
		float probability = static_cast<float>(distrib(randomEngine)) / 100.0f;
		float choice1 = totalFitness * probability;
		float choice2 = probability + 0.5f;
		if (choice2 >= 1.0f) {
			choice2 -= 1.0f;
		}
		choice2 *= totalFitness;


		// Loop over all the phrases
		for (Phrase* iter : phrases)
		{
			// Subtract fitness values from the choices to "iterate"
			// over the weighted choices
			choice1 -= iter->_fitnessValue;
			choice2 -= iter->_fitnessValue;

			// If choice variables are 0 or less, save the current value
			if (choice1 <= 0.0f && !selection1) {
				selection1 = iter;
			}
			if (choice2 <= 0.0f && !selection2) {
				selection2 = iter;
			}
		}

		if (!selection1 || !selection2) {

			std::cout << "Nullptr selected" << std::endl;
		}

		// Return the two elements as a pair
		return std::make_pair(selection1, selection2);
	}

	void TournamentSelection::SetNumRounds(unsigned numRounds) {

		m_numRounds = (numRounds > 0) ? numRounds : 1;
	}

	void TournamentSelection::SetParentPoolSize(unsigned numPossibleParents) {

		m_possibleParents = (numPossibleParents > 0) ? numPossibleParents : 1;
	}

	// Helper to run a single round of a tournament
	Phrase* RunTournament(const std::vector<Phrase*>& population, std::mt19937& randomEngine, int parents);

	BreedingPair TournamentSelection::Select(PhrasePool* population, std::mt19937& randomEngine) {

		Phrase* selection1 = nullptr;
		Phrase* selection2 = nullptr;

		const std::vector<Phrase*>& phraseList = population->GetPhrases();

		// Pick two parents by running two seperate tournaments
		// TODO: add support for multi round tournaments
		selection1 = RunTournament(phraseList, randomEngine, m_possibleParents);
		selection2 = RunTournament(phraseList, randomEngine, m_possibleParents);
		
		// Ensure we have two different parents
		while (selection1 == selection2) {

			selection2 = RunTournament(phraseList, randomEngine, m_possibleParents);
		}

		// Return the two parents as a pair
		return std::make_pair(selection1, selection2);
	}
	
	// Helper to run a single round of a tournament
	Phrase* RunTournament(const std::vector<Phrase*>& population, std::mt19937& randomEngine, int parents) {

		Phrase* best = nullptr;
		std::uniform_int_distribution<int> integers(0, static_cast<int>(population.size()));
		// Loop over the number of potential parents
		for (int k = 0; k < parents; ++k) {

			// Randomly select a parent from the population
			Phrase* candidate = population[integers(randomEngine)];

			// Check if it's better than the current best, and save it if so
			if (best == nullptr || best->_fitnessValue < candidate->_fitnessValue) {

				best = candidate;
			}
		}
	 
		// Return the best parent we could find in our tournament
		return best;
	}

} // namespace Genetics