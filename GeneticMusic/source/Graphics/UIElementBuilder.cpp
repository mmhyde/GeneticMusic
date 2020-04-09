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

		PianoRoll* pianoRoll = new PianoRoll(createPianoRollInterface(algorithm));
		gui->addUIElement(pianoRoll);
		
		RuleEditor* ruleEditor = new RuleEditor(createRuleManagerInterface());
		gui->addUIElement(ruleEditor);

		PhraseListViewer* phraseList = new PhraseListViewer(createSelectorInterface(algorithm));
		gui->addUIElement(phraseList);

		PhrasePlaybackWindow* playbackWindow = new PhrasePlaybackWindow(createPlaybackInterface(algorithm));
		gui->addUIElement(playbackWindow);

		AlgorithmExecutionWindow* executionWindow = new AlgorithmExecutionWindow(createAlgorithmExecutionInterface(algorithm));
		gui->addUIElement(executionWindow);
	}

} // namespace Genetics