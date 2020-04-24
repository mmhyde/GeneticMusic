/*
** Author  - Morgen Hyde
** Project - MusicGenetics
** Date    - 10 / 21 / 2019
*/

#include "Generation/PopulationGenerator.h"
#include "PhrasePool.h"
#include "ChordDefinitions.h"

#include <iostream>
#include <iomanip>
#include <random>

namespace Genetics {

	PopulationGenerator::PopulationGenerator(uint32_t populationSize, const PhraseConfig& heuristics)
		: m_configuration(heuristics), m_populationSize(populationSize), 
		  m_phraseAllocator(new PoolAllocator<Phrase>(2 * populationSize + 1)) {

		std::random_device rd;
		m_randomEngine.seed(rd());
	}
	
	PopulationGenerator::~PopulationGenerator() {
	}

	void PopulationGenerator::resetAllocator(uint32_t newSize) {
		
		if (newSize != 0) {
			m_populationSize = newSize;
		}

		delete m_phraseAllocator;
		m_phraseAllocator = new PoolAllocator<Phrase>(2 * m_populationSize + 1);
	}

	unsigned PopulationGenerator::GetPopulationSize() const
	{
		return m_populationSize;
	}

	PhrasePool* PopulationGenerator::GeneratePopulation()
	{
		PhrasePool* newPhrasePool = new PhrasePool(m_phraseAllocator, 
			m_configuration.numMeasures, m_configuration.smallestSubdivision);

		Phrase::_numMeasures = m_configuration.numMeasures;
		Phrase::_smallestSubdivision = m_configuration.smallestSubdivision;

		for (unsigned i = 0; i < m_populationSize; ++i)
		{
			Phrase* newPhrase = newPhrasePool->AllocateChild();

			// Logic for how to fill the phrase with actual notes goes here
			unsigned phraseLen = m_configuration.numMeasures;
			unsigned smallestSubDiv = m_configuration.smallestSubdivision;
			
			// Generate initial melody notes
			generateMelodic(newPhrase, phraseLen, smallestSubDiv);

			// Generate initial harmonic notes
			generateHarmonic(newPhrase, phraseLen, smallestSubDiv);
		}

		newPhrasePool->MergeChildrenToPopulation<GenerationalPrune>();

		return newPhrasePool;
	}


	void PopulationGenerator::generateMelodic(Phrase* phrase, unsigned numMeasures, unsigned subDiv)
	{
		// 1 - Generate Melodic Rhythm (use version of ddm from Langston paper)
		
		// First param is how many of whatever the smallest subdivision is we have per measure
		// Second param is the multiplier we add to the density each recursive layer
		// Third param is smallest subdivision we want to allow in the measure (as a integer number of the actual smallest)
		// For example to get 4 4 with smallest allowed to be 16th notes, we have 16, something, 1 with the subDiv = 16
		SubdivisionInfo subDivInfo = { static_cast<short>(subDiv), 1.55f, 1 };

		unsigned int maxNotes = numMeasures * subDiv;

		std::vector<char> phraseRhythm;
		std::vector<char> measureRhythm;

		// Reserve space for the literal worst rhythm to speed up push_back(s)
		phraseRhythm.reserve(maxNotes);

		for(unsigned i = 0; i < numMeasures; ++i)
		{
			subdivisionPattern(measureRhythm, subDivInfo, 0, 0.15f);

			for (int j = 0; j < measureRhythm.size(); ++j) {
			//	std::cout << static_cast<int>(measureRhythm[j]) << ", ";
				phraseRhythm.push_back(measureRhythm[j]);
			}
			//std::cout << std::endl;

			measureRhythm.clear();
		}

		size_t numNotes = phraseRhythm.size();

		// 2 - Generate Melodic Pitches
		// For now this is going to be a purely random distribution to see if the genetic algorithm can actually
		// optimize out bad note sequences
	
		std::vector<char> melodicPitches;
		melodicPitches.reserve(numNotes);

		std::uniform_int_distribution<int> distrib(MinPitch, MaxPitch);
		for (unsigned i = 0; i < numNotes; ++i) {
			melodicPitches.push_back(distrib(m_randomEngine));
		}

		phrase->_melodicNotes = static_cast<unsigned>(numNotes);

		// Loop over the notes and copy them into the phrase
		for (unsigned index = 0, note = 0; index < maxNotes, note < numNotes; ++note) {

			phrase->_melodicData[index] = melodicPitches[note];
			phrase->_melodicRhythm[index] = phraseRhythm[note];

			index += phraseRhythm[note];
		}
	}

	void PopulationGenerator::generateHarmonic(Phrase* phrase, unsigned numMeasures, unsigned subDiv) {

		// Temp Solution:

		// Hardcoded harmonic progression that only moves every quarter notes
		// First pass uses a basic I-IV-V-I

		// Determine number of quarter notes so we know how many chords to generate
		uint32_t numNotes = numMeasures * ChordRhythm;

		phrase->_harmonicNotes = numNotes;

		// Generate initial progression
		for (uint32_t measure = 0; measure < numMeasures; ++measure) {

			uint32_t idx = ChordRhythm * measure;

			phrase->_harmonicData[idx + 0] = { ChordNumeral::I  }; // Defaults to Major
			phrase->_harmonicData[idx + 1] = { ChordNumeral::IV };
			phrase->_harmonicData[idx + 2] = { ChordNumeral::V  };
			phrase->_harmonicData[idx + 3] = { ChordNumeral::I  };
		}
	}

	void PopulationGenerator::subdivisionPattern(std::vector<char>& pattern, const SubdivisionInfo& info, short layer, float density)
	{
		// Calculate a number from 0 to 1
		std::uniform_int_distribution<int> distrib(0, 100);
		float probability = (float)(distrib(m_randomEngine)) / 100.0f; 

		// If that number is larger than the density & we can subdivide further, 
		// recurse down twice to subdivide the current note
		if (probability > density && info.length >> layer > info.subDivMin) 
		{
			subdivisionPattern(pattern, info, layer + 1, density * info.layerMod);
			subdivisionPattern(pattern, info, layer + 1, density * (info.layerMod + 0.2f));
		}
		else // If we didn't meet the condition to recurse down a layer then just add whatever our current size is
		{
			pattern.push_back(info.length >> layer);
		}
	}

} // namespace Genetics