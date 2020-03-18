#pragma once

#include "PhrasePool.h"

#include <iostream>

namespace Genetics {

	struct Phrase;
	class PhrasePool;
	class ExtractorBase;

	// Policy classes for how fitness should occur
	struct ManualFitness {

	protected:
		ManualFitness() = default;
		virtual ~ManualFitness() = default;

		void evaluate(Phrase* phrase);
	};

	struct AutomaticFitness {

	protected:
		AutomaticFitness();
		virtual ~AutomaticFitness();

		void evaluate(Phrase* phrase);

	private:
		std::vector<ExtractorBase*> m_extractorList;
	};

	// Policy host, main object interacted with during algorithm operation
	template <class FitnessPolicy = ManualFitness>
	class FitnessEvaluator : public FitnessPolicy {

	public:
		FitnessEvaluator() = default;
		~FitnessEvaluator() = default;

		void Assess(PhrasePool* phrasePopulation);
		void Assess(Phrase* phrase);
	};

	template <class FitnessPolicy>
	void FitnessEvaluator<FitnessPolicy>::Assess(PhrasePool* phrasePopulation) {

		const std::vector<Phrase*>& population = phrasePopulation->GetPhrases();
		int phraseLen = phrasePopulation->GetMeasuresPerPhrase();
		int subDiv = phrasePopulation->GetSmallestSubDivision();

		for (Phrase* iter : population) {
			FitnessPolicy::evaluate(iter);
		}
	}

	template <class FitnessPolicy>
	void FitnessEvaluator<FitnessPolicy>::Assess(Phrase* phrase) {

		FitnessPolicy::evaluate(phrase);
	}


} // namespace Genetics