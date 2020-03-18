// Morgen Hyde

#include "Graphics/PhrasePlaybackWindow.h"
#include "GAControllerInterfaces.h"

#include "imgui/imgui.h"

namespace Genetics {

	const float xPaddingOffset = 8.0f;

	PhrasePlaybackWindow::PhrasePlaybackWindow(PhrasePlaybackInterface* _interface)
		: m_interface(_interface) {

	}

	PhrasePlaybackWindow::~PhrasePlaybackWindow() {
		
		delete m_interface;
	}

	void PhrasePlaybackWindow::render() {

		// Super simple :]
		// For this window we need:
		ImVec2 buttonDim(ImGui::GetContentRegionAvail().x, 0.0f);

		// Play button
		if (ImGui::Button("Play", buttonDim)) {

			m_interface->m_playActivePhrase();
		}

		// Pause button
		//if (ImGui::Button("Pause", ImVec2((buttonDim.x-xPaddingOffset) / 2.0f, 0.0f))) {
		//
		//	m_interface->m_pauseActivePhrase();
		//}
		//
		//ImGui::SameLine();

		// Stop button
		if (ImGui::Button("Stop", buttonDim)) {

			m_interface->m_stopActivePhrase();
		}
		
		// Optional Extra: Cycle output synth
	
	}

} // namespace Genetics