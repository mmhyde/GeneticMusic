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

		unsigned GetPopulationSize() const;
		PhrasePool* GeneratePopulation();
	
	private:

		struct SubdivisionInfo
		{
			short length;
			float layerMod;
			short subDivMin;
		};

		void GenerateMelodic(Phrase* phrase, unsigned phraseLen, unsigned subDiv);
		void GenerateHarmonic(Phrase* phrase, unsigned phraseLen, unsigned subDiv);

		void SubdivisionPattern(std::vector<char>& pattern, const SubdivisionInfo& info, short layer, float density);

		PhraseConfig m_configuration;
		unsigned m_populationSize;

		PoolAllocator<Phrase> m_phraseAllocator;
		std::mt19937 m_randomEngine;
	};


} // namepsace Genetics