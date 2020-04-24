#pragma once

#include "PoolAllocator.h"
#include "GADefaultConfig.h"

#include <random>

namespace Genetics {
	
	
	class PhrasePool;
	struct Phrase;

	// TODO: Make this a policy host with policies for melodic, harmonic, and rhythmic generation
	class PopulationGenerator
	{
	public:
		PopulationGenerator(unsigned populationSize, const PhraseConfig& configuration);
		~PopulationGenerator();

		void resetAllocator(uint32_t newSize = 0);

		unsigned GetPopulationSize() const;
		PhrasePool* GeneratePopulation();
	
	private:

		struct SubdivisionInfo
		{
			short length;
			float layerMod;
			short subDivMin;
		};

		void generateMelodic(Phrase* phrase, unsigned numMeasures, unsigned subDiv);
		void generateHarmonic(Phrase* phrase, unsigned numMeasures, unsigned subDiv);

		void subdivisionPattern(std::vector<char>& pattern, const SubdivisionInfo& info, short layer, float density);

		PhraseConfig m_configuration;
		unsigned m_populationSize;

		PoolAllocator<Phrase>* m_phraseAllocator;
		std::mt19937 m_randomEngine;
	};


} // namepsace Genetics