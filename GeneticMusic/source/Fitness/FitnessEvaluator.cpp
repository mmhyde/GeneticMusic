/*
** Author  - Morgen Hyde
** Project - MusicGenetics
** Date    - 10 / 23 / 2019
*/

#include "Fitness/FitnessEvaluator.h"
#include "Fitness/RuleManager.h"

#include <iostream>
#include <iomanip>
#include <cmath>

namespace Genetics {

	void ManualFitness::evaluate(Phrase* phrase) {

		unsigned subDivision = phrase->_smallestSubdivision;
		unsigned measureCount = phrase->_numMeasures;

		std::cout << "-----Phrase #" << phrase->_phraseID << "-----" << std::endl;
		std::cout << std::setw(3);

		int lastIndex = 0;

		// Setup the temporary vectors for storing information
		std::vector<int> phraseMelody;
		phraseMelody.reserve(subDivision);

		std::vector<int> melodyRhythm;
		melodyRhythm.reserve(subDivision);

		// Get the expected number of notes in the entire phrase
		unsigned noteCount = phrase->_melodicNotes;

		// Loop over each measure in the phrase
		for (unsigned measureNum = 0; measureNum < measureCount; ++measureNum) {

			// Loop over the measure, accumulating the values in the two array into our temp vectors
			for (unsigned count = 0; count < subDivision;) {

				phraseMelody.push_back(phrase->_melodicData[lastIndex]);
				melodyRhythm.push_back(phrase->_melodicRhythm[lastIndex]);

				lastIndex += melodyRhythm.back();
				count     += melodyRhythm.back();
			}

			// Print out gathered measure information

			std::cout << "Measure #" << measureNum << std::endl;
			std::cout << " | ";

			// Pitches
			for (const int& value : phraseMelody) {

				if (&value != &phraseMelody.front()) {
					std::cout << " - ";
				}
				std::cout << std::setw(3) << value; 
			}
			std::cout << " | " << std::endl;
			std::cout << " | ";

			// Rhythm Values
			for (const int& value : melodyRhythm) {

				if (&value != &melodyRhythm.front()) {
					std::cout << " - ";
				}
				std::cout << std::setw(3) << value;
			}
			std::cout << " | " << std::endl;

			phraseMelody.clear();
			melodyRhythm.clear();
		}

		std::cout << "Enter \'g\' if the phrase is good and \'b\' if the phrase is bad." << std::endl;
		char rating;
		std::cin >> rating;

		while (rating != 'g' && rating != 'b')
		{
			std::cout << "Please enter a valid rating (g or b)" << std::endl;
			std::cin >> rating;
		}

		float approval = 0.0f;
		if (rating == 'g') {
			approval = 1.0f;
		}

		phrase->_fitnessValue = (phrase->_fitnessValue + approval) / 2.0f;
	}

	AutomaticFitness::AutomaticFitness() {

		RuleManager& ruleManager = RuleManager::getRuleManager();
		ruleManager.createAllExtractors(m_extractorList);
	}

	AutomaticFitness::~AutomaticFitness() {

		for (ExtractorBase* extractor : m_extractorList) {
			delete extractor;
			extractor = nullptr;
		}
	}

#define FITNESS_EPSILON 0.001f

	void AutomaticFitness::evaluate(Phrase* phrase) {

		ExtractorBase* pitchExtractor = m_extractorList[ext_Pitch];
		float fitness = pitchExtractor->process(phrase);

		//float fitness = m_heuristic.AnalyzePhrase(phrase);
		if (fitness > 1.0f || std::isnan(fitness)) {
			std::cout << "problem phrase discovered" << std::endl;
		}
		if (fitness < FITNESS_EPSILON) {
			fitness = FITNESS_EPSILON;
		}

		phrase->_fitnessValue = fitness;
	}

} // namespace Genetics