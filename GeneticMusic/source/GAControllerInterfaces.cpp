// Morgen Hyde

#include "GAControllerInterfaces.h"

#include "GAController.h"
#include "Fitness/RuleManager.h"

namespace Genetics {

	RuleManagerInterface* createRuleManagerInterface() {

		RuleManager& manager = RuleManager::getRuleManager();
		RuleManagerInterface* interface_ = new RuleManagerInterface;

		interface_->m_getRuleTypes = RuleManagerInterface::TypeListGettor(&manager, &RuleManager::getRuleTypeList);
		interface_->m_getModifierTypes = RuleManagerInterface::TypeListGettor(&manager, &RuleManager::getModifierNameList);
		interface_->m_getFunctionNames = RuleManagerInterface::TypeListGettor(&manager, &RuleManager::getFunctionNameList);

		interface_->m_getAllRuleData = RuleManagerInterface::RuleInfoGettor(&manager, &RuleManager::getRuleData);

		interface_->m_getFunctionObject = RuleManagerInterface::FunctionGettor(&manager, &RuleManager::getFunctionHandle);

		interface_->m_setRuleInfo = RuleManagerInterface::RuleInfoSettor(&manager, &RuleManager::setRuleInfo);

		interface_->m_createRule = RuleManagerInterface::RuleCreator(&manager, &RuleManager::createRule);
		interface_->m_deleteRule = RuleManagerInterface::RuleDeleter(&manager, &RuleManager::removeRule);
		
		interface_->m_createFunction = RuleManagerInterface::FunctionCreator(&manager, &RuleManager::createNewFunction);
		interface_->m_deleteFunction = RuleManagerInterface::FunctionDeleter(&manager, &RuleManager::removeFunction);

		interface_->m_importRules = RuleManagerInterface::RuleIO(&manager, &RuleManager::importRules);
		interface_->m_exportRules = RuleManagerInterface::RuleIO(&manager, &RuleManager::exportRules);

		return interface_;
	}

	PianoRollInterface* createPianoRollInterface(GeneticAlgorithmController* controller) {

		typedef GeneticAlgorithmController GAC;
		typedef PianoRollInterface PRI;

		PRI* interface_ = new PRI;

		interface_->m_getActivePhrase = PRI::ActivePhraseGettor(controller, &GAC::getActivePhrase);

		return interface_;
	}

	PhraseSelectorInterface* createSelectorInterface(GeneticAlgorithmController* controller) {

		// Readability typedefs
		typedef GeneticAlgorithmController GAC;
		typedef PhraseSelectorInterface PSI;

		PSI* interface_ = new PSI;

		interface_->m_readPhraseList = PSI::PhraseListReader(controller, &GAC::getPhraseList);

		interface_->m_setActivePhrase = PSI::ActivePhraseSetter(controller, &GAC::setActivePhrase);
		interface_->m_getActivePhrase = PSI::ActivePhraseGetter(controller, &GAC::getActivePhrase);

		return interface_;
	}

	PhrasePlaybackInterface* createPlaybackInterface(GeneticAlgorithmController* controller) {

		// Readability typedefs
		typedef GeneticAlgorithmController GAC;
		typedef PhrasePlaybackInterface PPI;

		PPI* interface_ = new PPI;

		interface_->m_playActivePhrase = PPI::PhrasePlayback(controller, &GAC::playCurrentlySelectedPhrase);
		interface_->m_pauseActivePhrase = PPI::PhrasePlayback(controller, &GAC::pauseCurrentlySelectedPhrase);
		interface_->m_stopActivePhrase = PPI::PhrasePlayback(controller, &GAC::stopCurrentlySelectedPhrase);

		interface_->m_setPlaybackSynthesizer = PPI::SynthSetting(controller, &GAC::setPlaybackSynthesizer);

		return interface_;
	}

	AlgorithmExecutionInterface* createAlgorithmExecutionInterface(GeneticAlgorithmController* controller) {

		// Readability typedefs
		typedef GeneticAlgorithmController GAC;
		typedef AlgorithmExecutionInterface AEI;

		AEI* interface_ = new AEI;

		interface_->m_setIterationCount = AEI::AlgorithmSetter(controller, &GAC::setIterationCount);
		interface_->m_setPopulationSize = AEI::AlgorithmSetter(controller, &GAC::setPhrasePoolSize);

		interface_->m_setMeasureCount = AEI::AlgorithmSetter(controller, &GAC::setPhraseMeasureCount);
		interface_->m_setMeasureSmallestNote = AEI::AlgorithmSetter(controller, &GAC::setPhraseSmallestSubdivision);

		interface_->m_runAlgorithm = AEI::AlgorithmExecute(controller, &GAC::run);

		return interface_;
	}

} // namespace Genetics