// Morgen Hyde
#pragma once

#include "Graphics/UIElements.h"

namespace Genetics {

	struct AlgorithmExecutionInterface;

	class AlgorithmExecutionWindow : public UIElement {

	public:
		AlgorithmExecutionWindow(AlgorithmExecutionInterface* interface);
		~AlgorithmExecutionWindow();

		void render() override;

	private:
		AlgorithmExecutionInterface* m_interface;

	};


} // namespace Genetics