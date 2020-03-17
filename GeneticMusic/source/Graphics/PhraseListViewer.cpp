// Morgen Hyde

#include "Graphics/PhraseListViewer.h"
#include "GAControllerInterfaces.h"
#include "Phrase.h"

#include "imgui/imgui.h"

#include <string>

namespace Genetics {

	PhraseListViewer::PhraseListViewer(PhraseSelectorInterface* _interface)
		: m_interface(_interface) {

	}

	PhraseListViewer::~PhraseListViewer() {
		
		delete m_interface;
	}

	void PhraseListViewer::render() {

		// Get the up to date list of the phrases
		const std::vector<Phrase*>& phraseVec = m_interface->m_readPhraseList();
		uint32_t activePhraseID = m_interface->m_getActivePhrase()->_phraseID;

		ImGui::Text("Currently Generated Phrase List");
		ImGui::Text("Displaying Phrase: %d", activePhraseID);

		ImGui::Separator();

		ImVec2 buttonDim(ImGui::GetContentRegionAvail().x, 0.0f);

		// For each phrase in the list
		ImGui::BeginChild("PhraseScrolling");
		for (Phrase* phrase : phraseVec) {
			
			// Draw a tile / button for it 
			std::string buttonText("Phrase ID: ");
			
			// Button should hold:

			// - Phrase ID
			uint32_t phraseID = phrase->_phraseID;
			buttonText += std::to_string(phraseID);
			buttonText += "\nFitness Score: ";

			// - Fitness Score
			float phraseScore = phrase->_fitnessValue;
			std::string scoreString = std::to_string(phraseScore);
			buttonText += scoreString.substr(0, 4);

			// If the button is displaying the currently active phrase
			bool selected = (activePhraseID == phraseID);
			if (selected) {

				// Render it a lighter color
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0, 153, 153));
			}
			ImGui::PushID(phraseID);

			// If the button is clicked on
			if (ImGui::Button(buttonText.c_str(), buttonDim)) {
				
				// Set the current phrase ID to be the new active phrase
				activePhraseID = phraseID;
				m_interface->m_setActivePhrase(phraseID);
			}

			ImGui::PopID();

			if (selected) {
				ImGui::PopStyleColor();
			}
		}
		ImGui::EndChild();
	}


} // namespace Genetics