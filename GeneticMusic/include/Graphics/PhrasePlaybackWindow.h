// Morgen Hyde
#pragma once

#include "UIElements.h"

namespace Genetics {

	struct PhrasePlaybackInterface;

	class PhrasePlaybackWindow : public UIElement {

	public:
		PhrasePlaybackWindow(PhrasePlaybackInterface* interface);
		~PhrasePlaybackWindow();

		void render() override;

	private:
		PhrasePlaybackInterface* m_interface;

	};

} // namespace Genetics