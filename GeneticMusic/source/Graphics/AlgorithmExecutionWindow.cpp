// Morgen Hyde

#include "Graphics/AlgorithmExecutionWindow.h"
#include "GAControllerInterfaces.h"

#include "imgui/imgui.h"

#include <cstdint>
#include <string>

namespace Genetics {

	AlgorithmExecutionWindow::AlgorithmExecutionWindow(AlgorithmExecutionInterface* _interface)
		: m_interface(_interface) {

	}

	AlgorithmExecutionWindow::~AlgorithmExecutionWindow() {

	}

	uint16_t ValidSubdivisions[] = { 1, 2, 4, 8, 16 };

	enum subdivions {
		enm_whole = 0,
		enm_half,
		enm_quarter,
		enm_eighth,
		enm_sixteenth,
		enm_max,
	};

	void renderDropDown(int& currentElement, const std::string& label) {

		uint16_t previewElem = ValidSubdivisions[currentElement];
		ImGui::Text(label.c_str());
		if (ImGui::BeginCombo("##l", std::to_string(previewElem).c_str())) {

			int numTypes = static_cast<int>(sizeof(ValidSubdivisions) / sizeof(uint16_t));
			for (int i = 0; i < numTypes; ++i) {

				uint16_t elem = ValidSubdivisions[i];
				bool currSelected = (i == currentElement);
				if (ImGui::Selectable(std::to_string(elem).c_str(), &currSelected)) {
					currentElement = i;
				}
			}
			ImGui::EndCombo();
		}
	}

	void AlgorithmExecutionWindow::render() {

		// For this window we need the following:

		ImVec2 buttonDim(ImGui::GetContentRegionAvail().x, 0.0f);

		// Start execution button
		if (ImGui::Button("Run", buttonDim)) {

			// NOTE: This will likely block and cause some issues since everything is single-threaded
			m_interface->m_runAlgorithm();
		}

		ImGui::PushItemWidth(buttonDim.x);

		// Integer input for number of generations
		static int numGenerations = 10;
		ImGui::Text("Generation Count");
		ImGui::Separator();
		if (ImGui::InputInt("##G", &numGenerations, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {

			m_interface->m_setIterationCount(static_cast<uint32_t>(numGenerations));
		}

		// Integer input for number of measures
		static int numMeasures = 4;
		ImGui::Text("Phrase length");
		ImGui::Separator();
		if (ImGui::InputInt("##P", &numMeasures, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {

			m_interface->m_setMeasureCount(static_cast<uint32_t>(numMeasures));
		}
		
		// Combo box for smallest subdivion (1, 2, 4, 8, 16)
		static int smallestSubdivIndex = enm_sixteenth;

		
		int originalVal = smallestSubdivIndex;
		renderDropDown(smallestSubdivIndex, "Smallest Subdivision");

		if (originalVal != smallestSubdivIndex) {
			m_interface->m_setMeasureSmallestNote(ValidSubdivisions[smallestSubdivIndex]);
		}

		ImGui::PopItemWidth();
	}


} // namespace Genetics