// Morgen Hyde
#pragma once

#include "UIElements.h"

namespace Genetics {

	struct PhraseSelectorInterface;

	class PhraseListViewer : public UIElement {

	public:
		PhraseListViewer(PhraseSelectorInterface* interface);
		~PhraseListViewer();

		void render() override;

	private:
		PhraseSelectorInterface* m_interface;

	};

} // namespace Genetics