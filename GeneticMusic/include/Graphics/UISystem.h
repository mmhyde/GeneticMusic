// Morgen Hyde
#pragma once

#include "Phrase.h"
#include <vector>

struct GLFWwindow;

namespace Genetics {

	class PianoRoll;
	class UIElement;

	class UISystem {

	public:

		UISystem();
		~UISystem();

		void initialize();
		void shutdown();

		void startFrame();
		void endFrame();

		void render();

		void addUIElement(UIElement* newElement) {
			m_elements.push_back(newElement);
		}

		__inline bool windowIsOpen() const {
			return !m_windowClose;
		}

	private:
		void pushAllStyleColors();
		void  popAllStyleColors();
		void renderUIColorPicker();

		std::vector<UIElement*> m_elements;

		GLFWwindow* m_window;
		int m_displayWidth, m_displayHeight;

		bool m_windowClose;

	};

} // namespace Genetics