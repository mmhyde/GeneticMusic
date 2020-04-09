// Morgen Hyde
#pragma once

#include "UIElements.h"
#include <memory>

namespace Genetics {

	struct PhrasePlaybackInterface;

	class PhrasePlaybackWindow : public UIElement {

	public:
		PhrasePlaybackWindow(std::unique_ptr<PhrasePlaybackInterface> interface);
		~PhrasePlaybackWindow();

		void render() override;

	private:
		std::unique_ptr<PhrasePlaybackInterface> m_interface;

	};

} // namespace Genetics