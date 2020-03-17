/*
** Author  - Morgen Hyde
** Project - MusicGenetics
** Date    - 9 / 29 / 2019
*/

// MiniProject Brief - The goal of this program is to use a genetic algorithm to
// generate melodic phrases of music as MIDI files. The algorithm relies on human 
// input to determine the "fitness" of any given musical phrase. See the research
// sources document for the papers by Al Biles and Andrew R. Brown, Toby Gifford,
// Robert Davidson, and Peter Langston for previous research this project uses

#include "Graphics/UISystem.h"
#include "Graphics/UIElementBuilder.h"

#include "GAController.h"
#include "GAControllerInterfaces.h"

#include "TemplateGarbage/VariadicUnion.h"

//#define TemplateTest

int main(int argc, char** argv) {

#ifdef TemplateTest

	Variadic<X, Y, Z> myVariadic;

	return 0;

#endif

	Genetics::GeneticAlgorithmController geneticAlgorithm;
	Genetics::UISystem GUI;

	// Create all the UI elements and pass them the appropriate interface elements
	Genetics::UIElementBuilder::createUIElements(&geneticAlgorithm, &GUI);

	// Initialize both the controller and UI
	geneticAlgorithm.initializeAlgorithm();
	GUI.initialize();

	// Start main loop
	while (GUI.windowIsOpen()) {

		GUI.render();
		geneticAlgorithm.updateAudioEngine();
	}
	
	// Shut everything down
	GUI.shutdown();
	geneticAlgorithm.shutdownAlgorithm();

	return 0;
}
