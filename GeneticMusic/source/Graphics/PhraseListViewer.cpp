// Morgen Hyde

#include "Graphics/PhraseListViewer.h"
#include "GAControllerInterfaces.h"
#include "Phrase.h"

#include "imgui/imgui.h"
#include "Graphics/ImGuiHelpers.h"

#include <string>

namespace Genetics {

	const std::string DefaultFileName("PlaceholderFilename\0\0\0\0\0\0\0\0\0\0\0");

	PhraseListViewer::PhraseListViewer(std::unique_ptr<PhraseSelectorInterface> _interface)
		: m_interface(std::move(_interface)) {

	}

	PhraseListViewer::~PhraseListViewer() {

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

			bool openImport = false;
			bool openExport = false;

			if (ImGui::BeginPopupContextItem("IO Dialog", 1)) {

				if (ImGui::MenuItem("Import MIDI")) { openImport = true; }

				if (ImGui::MenuItem("Export MIDI")) { openExport = true; }

				ImGui::EndPopup();
			}

			if (openImport) { 
			
				ImGui::OpenPopup("Import Dialog"); 
				ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size * 0.33f);
			}

			if (ImGui::BeginPopupModal("Import Dialog")) {

				static std::string importPath(DefaultFileName);

				bool enterHit = ImGui::InputText("FileName", &(importPath.front()), importPath.size(), ImGuiInputTextFlags_EnterReturnsTrue);

				if (enterHit || ImGui::Button("Import", ImVec2(-1, 0))) {

					std::string toLoad = importPath.substr(0, importPath.find('\0'));
					toLoad.append(".midi");

					m_interface->m_importMIDI("Assets//" + toLoad, phrase);

					ImGui::CloseCurrentPopup();
				}

				if (ImGui::Button("Cancel", ImVec2(-1, 0))) {

					ImGui::CloseCurrentPopup();
					importPath.erase();
					importPath.insert(0, DefaultFileName);
					importPath.resize(48);
				}

				ImGui::EndPopup();
			}

			if (openExport) { 

				ImGui::OpenPopup("Export Dialog");
				ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size * 0.33f);
			}

			if (ImGui::BeginPopupModal("Export Dialog")) {

				static std::string exportPath(DefaultFileName);

				bool enterHit = ImGui::InputText("FileName", &(exportPath.front()), exportPath.size(), ImGuiInputTextFlags_EnterReturnsTrue);

				if (enterHit || ImGui::Button("Export", ImVec2(-1, 0))) {

					std::string toLoad = exportPath.substr(0, exportPath.find('\0'));
					toLoad.append(".midi");

					m_interface->m_exportMIDI("Output//" + toLoad, phrase);

					ImGui::CloseCurrentPopup();
				}


				if (ImGui::Button("Cancel", ImVec2(-1, 0))) {

					ImGui::CloseCurrentPopup();
					exportPath.erase();
					exportPath.insert(0, DefaultFileName);
					exportPath.resize(48);
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			if (selected) {
				ImGui::PopStyleColor();
			}
		}
		ImGui::EndChild();
	}


} // namespace Genetics