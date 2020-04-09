// Morgen Hyde
#pragma once

#include "UIElements.h"
#include <memory>

namespace Genetics {

	struct PhraseSelectorInterface;

	class PhraseListViewer : public UIElement {

	public:
		PhraseListViewer(std::unique_ptr<PhraseSelectorInterface> interface);
		~PhraseListViewer();

		void render() override;

	private:
		std::unique_ptr<PhraseSelectorInterface> m_interface;

	};

} // namespace Genetics