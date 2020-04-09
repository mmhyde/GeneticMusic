
#include "GAController.h"
#include "GADefaultConfig.h"

#include "AudioPlayback/SynthesizerBase.h"
#include "FIleIO/MIDIFiles.h"


#ifdef _DEBUG
	#include "Utility/Diagnostics.h"
#endif

namespace Genetics {

	GeneticAlgorithmController::GeneticAlgorithmController()
		: m_populationGen(DefaultPopulationSize, { DefaultMeasureCount, DefaultSubdivision }),
		  m_phrasePool(nullptr), m_activePhrase(nullptr), m_iterationsPerStep(DefaultGenCount),
		  m_totalGenerations(0), m_activeSynth(nullptr), 
		  m_fitness() {

		m_phrasePool = m_populationGen.GeneratePopulation();
		m_activePhrase = m_phrasePool->GetPhrases().front();

		m_activeSynth = new SynthesizerBase(44100, DefaultMeter);
	}

	GeneticAlgorithmController::~GeneticAlgorithmController() {


	}

	void GeneticAlgorithmController::initializeAlgorithm() {

		m_audioEngine.Initialize();
		m_audioEngine.SetSynthesizer(m_activeSynth);

		m_fitness.Assess(m_phrasePool);
		m_mutation.InitMutationPool();
	}

	void GeneticAlgorithmController::run() {

		int16_t genCount = 0;
		uint32_t maxPopulation = m_populationGen.GetPopulationSize();
		while (genCount++ < m_iterationsPerStep) {

			// Check if we've generated enough children to fill a generation
			while (m_phrasePool->GetNumChildren() < maxPopulation) {

#ifdef _DEBUG
				//std::cout << "Starting selection step..." << std::endl;
#endif
				// Select a new set of parents
				BreedingPair selected = m_selection.SelectPair(m_phrasePool);

#ifdef _DEBUG
				//std::cout << "Starting breeding step..." << std::endl;
#endif
				// Breed the phrases together and produce an output (auto added as child in pool)
				m_breeding.Breed(selected, m_phrasePool);

#ifdef _DEBUG
				GA_Error errorCode = validateNoteCount(m_phrasePool->GetChildren().back());
				//printErrorMessage(errorCode);

				errorCode = validateNoteLengths(m_phrasePool->GetChildren().back());
				//printErrorMessage(errorCode);

				//std::cout << "Starting mutation step..." << std::endl;
#endif
				// Apply a mutation to the child to introduce some variety
				m_mutation.Mutate(m_phrasePool->GetChildren().back());

#ifdef _DEBUG
				errorCode = validateNoteCount(m_phrasePool->GetChildren().back());
				printErrorMessage(errorCode);

				errorCode = validateNoteLengths(m_phrasePool->GetChildren().back());
				printErrorMessage(errorCode);

				//std::cout << "Starting assessment step..." << std::endl;
#endif 
				// Evaluate the fitness of the new phrase
				m_fitness.Assess(m_phrasePool->GetChildren().back());
			}

			// Once we have enough children, prune the population back 
			m_phrasePool->MergeChildrenToPopulation<PruningType>();

			// Increment total generations counter
			++m_totalGenerations;
		}

		m_activePhrase = m_phrasePool->GetPhrases()[0];
	}

	void GeneticAlgorithmController::updateAudioEngine() {
		
		m_audioEngine.Update();
	}

	void GeneticAlgorithmController::shutdownAlgorithm() {

		m_audioEngine.StopAll();
		m_audioEngine.Shutdown();
	}


	void GeneticAlgorithmController::setMeterInfoStruct(const MeterInfo& meterData) {

		
	}

	void GeneticAlgorithmController::setPhrasePoolSize(uint32_t populationSize) {

	}

	void GeneticAlgorithmController::setPhraseMeasureCount(uint32_t measureCount) {

	}

	void GeneticAlgorithmController::setPhraseSmallestSubdivision(uint32_t subDivision) {

	}


	void GeneticAlgorithmController::setIterationCount(uint32_t iterations) {

		m_iterationsPerStep = iterations;
	}


	void GeneticAlgorithmController::setActivePhrase(uint32_t phraseID) {

		const PhraseVec& phrases = m_phrasePool->GetPhrases();
		for (Phrase* phrase : phrases) {
			
			if (phrase->_phraseID == phraseID) {
				m_activePhrase = phrase;
				return;
			}
		}
	}


	uint32_t GeneticAlgorithmController::getActivePhraseID() const {

		return m_activePhrase->_phraseID;
	}

	Phrase* GeneticAlgorithmController::getActivePhrase() const {

		return m_activePhrase;
	}


	const std::vector<Phrase*>& GeneticAlgorithmController::getPhraseList() const {

		return m_phrasePool->GetPhrases();
	}


	void GeneticAlgorithmController::exportPhraseToMIDI(const std::string& filepath, Phrase* phrase) const {

		MIDIHandler midiOutput;
		phrase = (phrase == nullptr) ? m_activePhrase : phrase;

		midiOutput.writeToMIDI(m_activePhrase, filepath);
	}

	void GeneticAlgorithmController::importMIDIToPhrase(const std::string& filepath, Phrase* phrase) {

		MIDIHandler midiInput;

		phrase = (phrase == nullptr) ? m_activePhrase : phrase;
		
		std::memset(phrase->_melodicData, 0, phrase->_smallestSubdivision * phrase->_numMeasures);
		std::memset(phrase->_melodicRhythm, 0, phrase->_smallestSubdivision * phrase->_numMeasures);
		phrase->_melodicNotes = 0;

		midiInput.readFromMIDI(m_activePhrase, filepath);
	}


	void GeneticAlgorithmController::playCurrentlySelectedPhrase() {
	
		m_audioEngine.Play(m_activePhrase);
	}

	void GeneticAlgorithmController::pauseCurrentlySelectedPhrase() {

	}

	void GeneticAlgorithmController::stopCurrentlySelectedPhrase() {

		m_audioEngine.StopAll();
	}


	void GeneticAlgorithmController::setPlaybackSynthesizer(SynthesizerBase* synth) {

		if (m_activeSynth) {
			delete m_activeSynth;
		}

		m_activeSynth = synth;
	}

	void GeneticAlgorithmController::resetPopulation() {

	}


} // namespace Genetics