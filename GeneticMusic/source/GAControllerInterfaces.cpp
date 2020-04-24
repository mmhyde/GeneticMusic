// Morgen Hyde

#include "GAControllerInterfaces.h"

#include "GAController.h"
#include "Fitness/RuleManager.h"

namespace Genetics {

	std::unique_ptr<RuleManagerInterface> createRuleManagerInterface() {

		RuleManager& manager = RuleManager::getRuleManager();
		std::unique_ptr<RuleManagerInterface> interface_ = std::make_unique<RuleManagerInterface>();

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

		sizeof(interface_->m_importRules);

		return interface_;
	}

	std::unique_ptr<PianoRollInterface> createPianoRollInterface(GeneticAlgorithmController* controller) {

		typedef GeneticAlgorithmController GAC;
		typedef PianoRollInterface PRI;

		std::unique_ptr<PRI> interface_ = std::make_unique<PRI>();

		interface_->m_getActivePhrase = PRI::ActivePhraseGettor(controller, &GAC::getActivePhrase);

		return interface_;
	}

	std::unique_ptr<PhraseSelectorInterface> createSelectorInterface(GeneticAlgorithmController* controller) {

		// Readability typedefs
		typedef GeneticAlgorithmController GAC;
		typedef PhraseSelectorInterface PSI;

		std::unique_ptr<PSI> interface_ = std::make_unique<PSI>();

		interface_->m_readPhraseList = PSI::PhraseListReader(controller, &GAC::getPhraseList);

		interface_->m_setActivePhrase = PSI::ActivePhraseSetter(controller, &GAC::setActivePhrase);
		interface_->m_getActivePhrase = PSI::ActivePhraseGetter(controller, &GAC::getActivePhrase);

		interface_->m_importMIDI = PSI::MIDIImporter(controller, &GAC::importMIDIToPhrase);
		interface_->m_exportMIDI = PSI::MIDIExporter(controller, &GAC::exportPhraseToMIDI);

		return interface_;
	}

	std::unique_ptr<PhrasePlaybackInterface> createPlaybackInterface(GeneticAlgorithmController* controller) {

		// Readability typedefs
		typedef GeneticAlgorithmController GAC;
		typedef PhrasePlaybackInterface PPI;

		std::unique_ptr<PPI> interface_ = std::make_unique<PPI>();

		interface_->m_playActivePhrase = PPI::PhrasePlayback(controller, &GAC::playCurrentlySelectedPhrase);
		interface_->m_pauseActivePhrase = PPI::PhrasePlayback(controller, &GAC::pauseCurrentlySelectedPhrase);
		interface_->m_stopActivePhrase = PPI::PhrasePlayback(controller, &GAC::stopCurrentlySelectedPhrase);

		interface_->m_setPlaybackSynthesizer = PPI::SynthSetting(controller, &GAC::setPlaybackSynthesizer);

		return interface_;
	}

	std::unique_ptr<AlgorithmExecutionInterface> createAlgorithmExecutionInterface(GeneticAlgorithmController* controller) {

		// Readability typedefs
		typedef GeneticAlgorithmController GAC;
		typedef AlgorithmExecutionInterface AEI;

		std::unique_ptr<AEI> interface_ = std::make_unique<AEI>();

		interface_->m_setIterationCount = AEI::AlgorithmSetter(controller, &GAC::setIterationCount);
		interface_->m_setPopulationSize = AEI::AlgorithmSetter(controller, &GAC::setPhrasePoolSize);

		interface_->m_clearPhrasePool = AEI::AlgorithmClear(controller, &GAC::clearPhrasePool);

		interface_->m_runAlgorithm = AEI::AlgorithmExecute(controller, &GAC::run);

		return interface_;
	}

} // namespace Genetics