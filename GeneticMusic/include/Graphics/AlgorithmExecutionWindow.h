// Morgen Hyde
#pragma once

#include "Graphics/UIElements.h"
#include <memory>

namespace Genetics {

	struct AlgorithmExecutionInterface;

	class AlgorithmExecutionWindow : public UIElement {

	public:
		AlgorithmExecutionWindow(std::unique_ptr<AlgorithmExecutionInterface> interface);
		~AlgorithmExecutionWindow();

		void render() override;

	private:
		std::unique_ptr<AlgorithmExecutionInterface> m_interface;

	};


} // namespace Genetics