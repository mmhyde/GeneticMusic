#pragma once

#include "Generation/PopulationGenerator.h"
#include "PolicyDefinitions.h"

#include "AudioPlayback/AudioEngine.h"

#include <vector>
#include <cstdint>

namespace Genetics {

	class SynthesizerBase;

	struct Phrase;
	struct MeterInfo;

	class GeneticAlgorithmController {

	public:
		GeneticAlgorithmController();
		~GeneticAlgorithmController();

		GeneticAlgorithmController(const GeneticAlgorithmController& rhs) = delete;
		GeneticAlgorithmController& operator=(const GeneticAlgorithmController& rhs) = delete;
		
		// State Management Functions //
		
		void initializeAlgorithm();
		void run();
		void updateAudioEngine();
		void shutdownAlgorithm();

		void setMeterInfoStruct(const MeterInfo& meterData);
		void setPhrasePoolSize(uint32_t populationSize);
		
		void clearPhrasePool();
		void setIterationCount(uint32_t iterations);

		// Phrase Manipulation Functions //

		void setActivePhrase(uint32_t phraseID);

		uint32_t getActivePhraseID() const;
		Phrase* getActivePhrase() const;

		const std::vector<Phrase*>& getPhraseList() const;

		void exportPhraseToMIDI(const std::string& filepath, Phrase* phrase = nullptr) const;
		void importMIDIToPhrase(const std::string& filepath, Phrase* phrase = nullptr);
		
		// Audio Playback Functions //

		void playCurrentlySelectedPhrase();
		void pauseCurrentlySelectedPhrase();
		void stopCurrentlySelectedPhrase();

		void setPlaybackSynthesizer(SynthesizerBase* synth);

	private:

		// Phrase and population
		PopulationGenerator m_populationGen;
		PhrasePool* m_phrasePool;
		Phrase* m_activePhrase;

		int16_t m_iterationsPerStep;
		uint32_t m_totalGenerations;
		uint32_t m_populationSize;

		// Audio playback
		SynthesizerBase* m_activeSynth;
		AudioEngine m_audioEngine;

		// Algorithm steps
		
		SelectionType m_selection;
		BreederType m_breeding;
		Mutator m_mutation;
		FitnessType m_fitness;
		
	};


} // namespace Genetics