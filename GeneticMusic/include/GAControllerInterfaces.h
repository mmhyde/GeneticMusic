// Morgen Hyde
#pragma once

#include "GenericFunctor.h"
#include "Fitness/RuleTypes.h"

#include <vector>

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

	RuleManagerInterface* createRuleManagerInterface();

	struct PianoRollInterface {

		typedef Functor<Phrase*> ActivePhraseGettor;

		ActivePhraseGettor m_getActivePhrase;
	};

	PianoRollInterface* createPianoRollInterface(GeneticAlgorithmController* controller);

	struct PhraseSelectorInterface {

		typedef Functor<const std::vector<Phrase*>&> PhraseListReader;
		typedef Functor<void, uint32_t> ActivePhraseSetter;
		typedef Functor<Phrase*> ActivePhraseGetter;

		PhraseListReader m_readPhraseList;

		ActivePhraseSetter m_setActivePhrase;
		ActivePhraseGetter m_getActivePhrase;
	};

	PhraseSelectorInterface* createSelectorInterface(GeneticAlgorithmController* controller);

	struct PhrasePlaybackInterface {

		typedef Functor<void> PhrasePlayback;
		typedef Functor<void, SynthesizerBase*> SynthSetting;

		PhrasePlayback m_playActivePhrase;
		PhrasePlayback m_pauseActivePhrase;
		PhrasePlayback m_stopActivePhrase;

		SynthSetting m_setPlaybackSynthesizer;
	};

	PhrasePlaybackInterface* createPlaybackInterface(GeneticAlgorithmController* controller);

	struct AlgorithmExecutionInterface {

		typedef Functor<void, uint32_t> AlgorithmSetter;
		typedef Functor<void> AlgorithmExecute;

		AlgorithmSetter m_setIterationCount;
		AlgorithmSetter m_setPopulationSize;

		AlgorithmSetter m_setMeasureCount;
		AlgorithmSetter m_setMeasureSmallestNote;

		AlgorithmExecute m_runAlgorithm;
	};

	AlgorithmExecutionInterface* createAlgorithmExecutionInterface(GeneticAlgorithmController* controller);

} // namespace Genetics