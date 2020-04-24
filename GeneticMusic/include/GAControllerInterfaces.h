// Morgen Hyde
#pragma once

#include "GenericFunctor.h"
#include "Fitness/RuleTypes.h"

#include <vector>
#include <memory>

namespace Genetics {

	class GeneticAlgorithmController;
	struct Phrase;
	class SynthesizerBase;

	struct RuleManagerInterface {

		// Gettor for lists of names: like types of rules, types of modifiers, fitness functions
		typedef Functor<const std::vector<std::string>&> TypeListGettor;

		// Gettor for registry of rules
		typedef Functor<const std::vector<RuleInfo>&> RuleInfoGettor;

		// Gettor for function data (modifiable! note it's a pointer)
		typedef Functor<std::shared_ptr<Function>, FunctionID> FunctionGettor;

		// Settor for rule information
		typedef Functor<void, const RuleInfo&> RuleInfoSettor;

		// Functor to create an empty rule
		typedef Functor<RuleID, RuleType, FunctionID> RuleCreator;

		// Functor to delete an existing rule
		typedef Functor<void, const RuleID&> RuleDeleter;

		// Functor to create an empty function
		typedef Functor<FunctionID, const std::string&> FunctionCreator;

		// Functor to delete an existing function
		typedef Functor<void, const FunctionID&> FunctionDeleter;

		// Functor to import/export a ruleset
		typedef Functor<bool, const std::string&> RuleIO;

		TypeListGettor m_getRuleTypes;
		TypeListGettor m_getModifierTypes;
		TypeListGettor m_getFunctionNames;
		RuleInfoGettor m_getAllRuleData;
		FunctionGettor m_getFunctionObject;

		RuleInfoSettor m_setRuleInfo;

		RuleCreator m_createRule;
		RuleDeleter m_deleteRule;

		FunctionCreator m_createFunction;
		FunctionDeleter m_deleteFunction;

		RuleIO m_exportRules;
		RuleIO m_importRules;
	};

	std::unique_ptr<RuleManagerInterface> createRuleManagerInterface();

	struct PianoRollInterface {

		typedef Functor<Phrase*> ActivePhraseGettor;

		ActivePhraseGettor m_getActivePhrase;
	};

	std::unique_ptr<PianoRollInterface> createPianoRollInterface(GeneticAlgorithmController* controller);

	struct PhraseSelectorInterface {

		typedef Functor<const std::vector<Phrase*>&> PhraseListReader;
		typedef Functor<void, uint32_t> ActivePhraseSetter;
		typedef Functor<Phrase*> ActivePhraseGetter;

		typedef Functor<void, const std::string&, Phrase*> MIDIImporter;
		typedef Functor<void, const std::string&, Phrase*> MIDIExporter;

		PhraseListReader m_readPhraseList;

		ActivePhraseSetter m_setActivePhrase;
		ActivePhraseGetter m_getActivePhrase;

		MIDIImporter m_importMIDI;
		MIDIExporter m_exportMIDI;
	};

	std::unique_ptr<PhraseSelectorInterface> createSelectorInterface(GeneticAlgorithmController* controller);

	struct PhrasePlaybackInterface {

		typedef Functor<void> PhrasePlayback;
		typedef Functor<void, SynthesizerBase*> SynthSetting;


		PhrasePlayback m_playActivePhrase;
		PhrasePlayback m_pauseActivePhrase;
		PhrasePlayback m_stopActivePhrase;

		SynthSetting m_setPlaybackSynthesizer;
	};

	std::unique_ptr<PhrasePlaybackInterface> createPlaybackInterface(GeneticAlgorithmController* controller);

	struct AlgorithmExecutionInterface {

		typedef Functor<void, uint32_t> AlgorithmSetter;
		typedef Functor<void> AlgorithmExecute;
		typedef Functor<void> AlgorithmClear;

		AlgorithmSetter m_setIterationCount;
		AlgorithmSetter m_setPopulationSize;

		AlgorithmClear m_clearPhrasePool;

		AlgorithmExecute m_runAlgorithm;
	};

	std::unique_ptr<AlgorithmExecutionInterface> createAlgorithmExecutionInterface(GeneticAlgorithmController* controller);

} // namespace Genetics