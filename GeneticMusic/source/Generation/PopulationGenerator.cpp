/*
** Author  - Morgen Hyde
** Project - MusicGenetics
** Date    - 10 / 21 / 2019
*/

#include "Generation/PopulationGenerator.h"
#include "PhrasePool.h"

#include <iostream>
#include <iomanip>
#include <random>

namespace Genetics {

	PopulationGenerator::PopulationGenerator(unsigned populationSize, const PhraseConfig& heuristics)
		: m_configuration(heuristics), m_populationSize(populationSize), 
		  m_phraseAllocator((2 * populationSize) + 1)
	{
		std::random_device rd;
		m_randomEngine.seed(rd());
	}
	
	PopulationGenerator::~PopulationGenerator()
	{
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

			// v Logic for how to fill the phrase with actual notes goes here v
			unsigned phraseLen = m_configuration.numMeasures;
			unsigned smallestSubDiv = m_configuration.smallestSubdivision;
			
			GenerateMelodic(newPhrase, phraseLen, smallestSubDiv);

			GenerateHarmonic(newPhrase, phraseLen, smallestSubDiv);

			// ^ ------------------------------------------------------------ ^
		}

		newPhrasePool->MergeChildrenToPopulation<GenerationalPrune>();

		return newPhrasePool;
	}



	void PopulationGenerator::GenerateMelodic(Phrase* phrase, unsigned phraseLen, unsigned subDiv)
	{
		// 1 - Generate Melodic Rhythm (use version of ddm from Langston paper)
		
		// First param is how many of whatever the smallest subdivision is we have per measure
		// Second param is the multiplier we add to the density each recursive layer
		// Third param is smallest subdivision we want to allow in the measure (as a integer number of the actual smallest)
		// For example to get 4 4 with smallest allowed to be 16th notes, we have 16, something, 1 with the subDiv = 16
		SubdivisionInfo subDivInfo = { static_cast<short>(subDiv), 1.55f, 1 };

		unsigned int maxNotes = phraseLen * subDiv;

		std::vector<char> phraseRhythm;
		std::vector<char> measureRhythm;

		// Reserve space for the literal worst rhythm to speed up push_back(s)
		phraseRhythm.reserve(maxNotes);

		for(unsigned i = 0; i < phraseLen; ++i)
		{
			SubdivisionPattern(measureRhythm, subDivInfo, 0, 0.15f);

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

	void PopulationGenerator::GenerateHarmonic(Phrase* phrase, unsigned phraseLeng, unsigned subDiv)
	{
		// 3 - Generate Harmonic Rhythm (based off melodic rhythm, simpler?)

		// 3 - Generate Harmonic Pitches (based off melodic pitches or vice versa)
	}

	void PopulationGenerator::SubdivisionPattern(std::vector<char>& pattern, const SubdivisionInfo& info, short layer, float density)
	{
		// Calculate a number from 0 to 1
		std::uniform_int_distribution<int> distrib(0, 100);
		float probability = (float)(distrib(m_randomEngine)) / 100.0f; 

		// If that number is larger than the density & we can subdivide further, 
		// recurse down twice to subdivide the current note
		if (probability > density && info.length >> layer > info.subDivMin) 
		{
			SubdivisionPattern(pattern, info, layer + 1, density * info.layerMod);
			SubdivisionPattern(pattern, info, layer + 1, density * (info.layerMod + 0.2f));
		}
		else // If we didn't meet the condition to recurse down a layer then just add whatever our current size is
		{
			pattern.push_back(info.length >> layer);
		}
	}

} // namespace Genetics