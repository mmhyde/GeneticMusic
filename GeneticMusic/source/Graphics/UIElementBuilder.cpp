// Morgen Hyde
#include "Graphics/UIElementBuilder.h"

#include "GAController.h"
#include "GAControllerInterfaces.h"

#include "Graphics/UISystem.h"

// UI element includes:
#include "Graphics/PianoRoll.h"
#include "Graphics/RuleEditor.h"
#include "Graphics/AlgorithmExecutionWindow.h"
#include "Graphics/PhraseListViewer.h"
#include "Graphics/PhrasePlaybackWindow.h"

namespace Genetics {

	void UIElementBuilder::createUIElements(GeneticAlgorithmController* algorithm, UISystem* gui) {

		PianoRollInterface* pianoInterface = createPianoRollInterface(algorithm);
		PianoRoll* pianoRoll = new PianoRoll(pianoInterface);

		gui->addUIElement(pianoRoll);
		
		std::unique_ptr<RuleManagerInterface> ruleInterface(createRuleManagerInterface());
		RuleEditor* ruleEditor = new RuleEditor(std::move(ruleInterface));

		gui->addUIElement(ruleEditor);

		PhraseSelectorInterface* selectorInterface = createSelectorInterface(algorithm);
		PhraseListViewer* phraseList = new PhraseListViewer(selectorInterface);

		gui->addUIElement(phraseList);

		PhrasePlaybackInterface* playbackInterface = createPlaybackInterface(algorithm);
		PhrasePlaybackWindow* playbackWindow = new PhrasePlaybackWindow(playbackInterface);

		gui->addUIElement(playbackWindow);

		AlgorithmExecutionInterface* executionInterface = createAlgorithmExecutionInterface(algorithm);
		AlgorithmExecutionWindow* executionWindow = new AlgorithmExecutionWindow(executionInterface);
	
		gui->addUIElement(executionWindow);
	}

} // namespace Genetics