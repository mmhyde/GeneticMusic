// Morgen Hyde

#pragma once

#include "Graphics/UIElements.h"
#include <memory>

namespace Genetics {

	struct Phrase;
	struct PianoRollInterface;

	class PianoRoll : public UIElement {

	public:
		PianoRoll(std::unique_ptr<PianoRollInterface> interface);
		~PianoRoll();

		void render() override;

	private:

		void DrawPianoKeys();
		void DrawTimelineGrid();
		void DrawPhraseOnGrid();
		void DrawScrollBars();

		float GetYCoordOfKey(short keyNumber) const;

		struct Key {

			void Render(const float& scrollingOffset);

			short m_noteValue;
			bool  m_isWhiteKey;
			float m_xPos, m_yPos;

			static float m_keyHeight, m_keyWidth;
		};

		float m_scrollingX, m_scrollingY;
		float m_gridThickness;
	
		std::unique_ptr<PianoRollInterface> m_interface;
		Key m_keyArray[128];

		bool m_mouseHoveringThisFrame;
	};

} // namespace Genetics