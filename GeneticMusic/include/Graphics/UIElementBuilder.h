// Morgen Hyde
#pragma once

namespace Genetics {

	class GeneticAlgorithmController;
	class UISystem;

	// Helper class to create all UI elements with appropriate interfaces
	// TODO: Expand this to just use factory pattern
	class UIElementBuilder {

	public:
		static void createUIElements(GeneticAlgorithmController* algorithm, UISystem* gui);
	};


} // namespace Genetics
