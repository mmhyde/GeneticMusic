// Morgen Hyde

#include "Graphics/RuleEditor.h"
#include "Fitness/FunctionBuilder.h"
#include "Graphics/ImGuiHelpers.h"

#include "GAControllerInterfaces.h"

#include "imgui/imgui_internal.h"

#include <string>
#include <sstream>
#include <iomanip>

#include <algorithm>

#define FIRST_VERT 0
#define SECOND_VERT 1
#define HASH_VERTEX(vertexAddr) (static_cast<uint64_t>(vertexAddr));

extern ImU32 backgroundColor;
extern ImU32 axisColor;
extern ImU32 gridLineColor;
extern ImU32 vertexColor;
extern ImU32 segmentColor;

extern ImU32 graphLabelColor;

namespace Genetics {

	const ImVec2 ruleListWindowRatio(0.3f, 1.0f);
	const ImVec2 dropDownWindowRatio(1.0f - ruleListWindowRatio.x, 0.2f);
	const ImVec2 functionWindowRatio(1.0f - ruleListWindowRatio.x, 1.0f - dropDownWindowRatio.y);

	const float xPaddingOffset = 9.0f;
	const float yPaddingOffset = 5.0f;

	const std::string DefaultFileName("UserRulesPreset\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

	RuleEditor::RuleEditor(std::unique_ptr<RuleManagerInterface> managerInterface)
		: m_interface(std::move(managerInterface)),
		m_ruleData(m_interface->m_getAllRuleData()),
		m_ruleTypeNames(m_interface->m_getRuleTypes()),
		m_modNames(m_interface->m_getModifierTypes()),
		m_functionNames(m_interface->m_getFunctionNames()) {

		m_activeRule = INVALID_RULE_ID;
	}

	RuleEditor::~RuleEditor() {

	}

	void RuleEditor::render() {

		// Left section of screen
		DrawRuleList();

		if (m_activeRule < INVALID_RULE_ID) {

			ImGui::SameLine();

			ImGui::BeginGroup();

			float width = ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPosX();
			width = (width - xPaddingOffset) / 2.0f;

			DrawFunctionButtons(width);

			// Top of right half
			DrawRuleDropdowns(width);

			// Bottom right, largest percentage of window contains this
			DrawFunctionEditor();

			ImGui::EndGroup();
		}
	}

	const std::string typePrefix("Rule Type: ");
	const std::string modPrefix("Modifier : ");
	const std::string functionPrefix("Function : ");

	const ImVec2 windowPadding(5.0f, 4.0f);

	void renderDropDown(uint16_t& currentElement, const std::vector<std::string>& elements, const std::string& label) {

		const std::string& previewElem = elements[currentElement];
		if (ImGui::BeginCombo(label.c_str(), previewElem.c_str())) {

			uint16_t numTypes = static_cast<uint16_t>(elements.size());
			for (uint8_t i = 0; i < numTypes; ++i) {

				const std::string& elem = elements[i];
				bool currSelected = (i == currentElement);
				if (ImGui::Selectable(elem.c_str(), &currSelected)) {
					currentElement = i;
				}
			}
			ImGui::EndCombo();
		}
	}


	void RuleEditor::DrawFunctionButtons(float width) {

		ImGui::BeginGroup();

		if (ImGui::Button("Create Function", ImVec2(width, 0))) {

			int newDuplicates = 0;
			for (const std::string& name : m_functionNames) {

				if (name.find("New Function") != std::string::npos) {
					++newDuplicates;
				}
			}

			m_interface->m_createFunction("New Function" + std::to_string(newDuplicates));
			m_interface->m_getFunctionNames(); // Update the reference we hold to the name vec
		}

		ImGui::SameLine();

		FunctionID targetID = DEFAULT_FUNCTION_ID;
		for (const RuleInfo& info : m_ruleData) {

			if (info.ruleID == m_activeRule) {

				targetID = info.funcID;
				break;
			}
		}
		std::shared_ptr<Function> original = m_interface->m_getFunctionObject(targetID);

		static std::string functionName;

		if (ImGui::Button("Copy Function", ImVec2(width, 0))) {

			// Create an empty function with a new ID and name
			std::string newName("Copy of " + original->getName());
			FunctionID copyID = m_interface->m_createFunction(newName);

			// Get the empty function and copy assign from the original
			std::shared_ptr<Function> copy = m_interface->m_getFunctionObject(copyID);
			*copy = *original;

			// Update reference to name vec
			m_interface->m_getFunctionNames();
		}

		if (ImGui::Button("Rename Function", ImVec2(width, 0))) {

			if (targetID != DEFAULT_FUNCTION_ID) {

				ImGui::OpenPopup("Rename Dialog");
				ImVec2 windowContent = ImGui::GetContentRegionAvail();
				ImGui::SetNextWindowSize(ImVec2(windowContent.x * 0.50f, windowContent.y * 0.30f));

				functionName.insert(0, original->getName());
				functionName.resize(64);
			}
		}
			
		ImGui::SameLine();
		
		if (ImGui::Button("Delete Function", ImVec2(width, 0))) {

			if (targetID != DEFAULT_FUNCTION_ID) {

				// Delete the function
				m_interface->m_deleteFunction(targetID);

				// Update the current rule's data about which function it now holds
				RuleInfo updatedInfo = m_ruleData[m_activeRule];
				updatedInfo.funcID = DEFAULT_FUNCTION_ID;

				// Send the data off to the manager
				m_interface->m_setRuleInfo(updatedInfo);
			}
		}

		ImGui::EndGroup();

		if (ImGui::BeginPopupModal("Rename Dialog")) {

			static bool displayErrorText = false;
			bool shouldClose = false;

			if (displayErrorText) {
				ImGui::Text("Function names must be unique and\ncontain more than 2 characters");
			}

			bool inputEnter = false;
			inputEnter = ImGui::InputText("Function Name", &(functionName.front()), functionName.size(), ImGuiInputTextFlags_EnterReturnsTrue);
			float buttonHeight = ImGui::GetTextLineHeight() * 1.5f;

			bool rename = ImGui::Button("Rename", ImVec2(-1, buttonHeight));
			bool cancel = ImGui::Button("Cancel", ImVec2(-1, buttonHeight));

			if (inputEnter || rename) {

				std::string newName = functionName.substr(0, functionName.find('\0'));
				
				std::vector<std::string>::const_iterator namesStart, namesStop;
				namesStart = m_functionNames.begin();
				namesStop = m_functionNames.end();

				// Verify uniqueness
				if (std::find(namesStart, namesStop, newName) != namesStop || newName.size() < 3) {
					
					displayErrorText = true;
				}
				else {

					original->setName(newName);

					// Force update
					m_interface->m_getFunctionNames();
					shouldClose = true;
				}
			}

			ImGui::SameLine();

			if (shouldClose || cancel) {

				functionName.erase();
				displayErrorText = false;

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}


	void RuleEditor::DrawRuleList() {

		ImVec2 maxCoord = ImGui::GetWindowContentRegionMax();
		ImVec2 minCoord = ImGui::GetWindowContentRegionMin();

		ImVec2 ruleListSize((maxCoord.x - minCoord.x) * ruleListWindowRatio.x,
			(maxCoord.y - minCoord.y) * ruleListWindowRatio.y);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, windowPadding);

		ImGui::BeginChild("Rule List", ruleListSize, true);

		ImVec2 buttonDim(ImGui::GetContentRegionAvail().x, 0.0f);

		if (ImGui::Button("Add Rule", buttonDim)) {

			ImGui::OpenPopup("Rule Creator");
			ImGui::SetNextWindowSize(ImVec2((maxCoord.x - minCoord.x) / 3.0f, (maxCoord.y - minCoord.y) / 3.0f));
		}

		bool open = true;
		if (ImGui::BeginPopupModal("Rule Creator", &open)) {

			static uint16_t ruleTypeIndex = 0;
			// Renders type name drop down
			renderDropDown(ruleTypeIndex, m_ruleTypeNames, "Rule Types");

			static uint16_t modifierTypeIndex = 0;
			renderDropDown(modifierTypeIndex, m_modNames, "Modifier Types");

			static uint16_t functionTypeIndex = 0;
			renderDropDown(functionTypeIndex, m_functionNames, "Functions");

			float buttonHeight = ImGui::GetTextLineHeight() * 1.5f;

			bool create = ImGui::Button("Create Rule", ImVec2(-1, buttonHeight));
			bool cancel = ImGui::Button("Cancel", ImVec2(-1, buttonHeight));

			if (create) {
				m_interface->m_createRule(static_cast<RuleType>(ruleTypeIndex), functionTypeIndex);
			}
			if (cancel || create) {

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		if (ImGui::Button("Import Rules", ImVec2((buttonDim.x / 2.0f) - (windowPadding.x / 2.0f), buttonDim.y))) {

			ImGui::OpenPopup("ImportDialog");
			ImGui::SetNextWindowSize(ImVec2((maxCoord.x - minCoord.x) / 3.0f, (maxCoord.y - minCoord.y) / 3.0f));
		}

		if (ImGui::BeginPopupModal("ImportDialog")) {

			static std::string importPath(DefaultFileName);
			static bool displayErrorText = false;

			if (displayErrorText) {
				ImGui::Text("Unable to import the specified file");
			}

			bool enterHit = ImGui::InputText("FileName", &(importPath.front()), importPath.size(), ImGuiInputTextFlags_EnterReturnsTrue);

			if (enterHit || ImGui::Button("Import")) { 

				std::string toLoad = importPath.substr(0, importPath.find('\0'));
				toLoad.append(".xml");

				displayErrorText = !m_interface->m_importRules("Output//" + toLoad);
				if (!displayErrorText) {
					
					importPath.erase();
					importPath.insert(0, DefaultFileName);
					importPath.resize(64);

					// Force update of the reference we hold to the function name list
					m_interface->m_getFunctionNames();

					// Reset the rule ID to avoid array access issues
					m_activeRule = INVALID_RULE_ID;

					ImGui::CloseCurrentPopup();
				}
			}
			
			if (ImGui::Button("Cancel")) { 
				
				ImGui::CloseCurrentPopup(); 
				importPath.erase();
				importPath.insert(0, DefaultFileName);
				importPath.resize(64);
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - windowPadding.x);

		if (ImGui::Button("Export Rules", ImVec2(buttonDim.x / 2.0f, buttonDim.y))) {

			ImGui::OpenPopup("ExportDialog");
			ImGui::SetNextWindowSize(ImVec2((maxCoord.x - minCoord.x) / 3.0f, (maxCoord.y - minCoord.y) / 3.0f));
		}

		if (ImGui::BeginPopupModal("ExportDialog")) {
			
			static std::string exportPath(DefaultFileName);
			static bool displayErrorText = false;

			if (displayErrorText) {
				ImGui::Text("Unable to export the specified file");
			}

			bool enterHit = ImGui::InputText("FileName", &(exportPath.front()), exportPath.size(), ImGuiInputTextFlags_EnterReturnsTrue);

			if (ImGui::Button("Export") || enterHit) {

				std::string toSave = exportPath.substr(0, exportPath.find('\0'));
				toSave.append(".xml");

				displayErrorText = !m_interface->m_exportRules("Output//" + toSave);
				if (!displayErrorText) {
					
					exportPath.erase();
					exportPath.insert(0, DefaultFileName);
					exportPath.resize(64);

					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::Button("Cancel")) {

				ImGui::CloseCurrentPopup();
				exportPath.erase();
				exportPath.insert(0, DefaultFileName);
				exportPath.resize(64);
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.0f);

		ImGui::Text("Rule List:");

		ImGui::Separator();

		ImGui::BeginChild("RuleScrolling");
		for (const RuleInfo& info : m_ruleData) {

			std::string typeString = typePrefix + m_ruleTypeNames[hashIDToType(info.ruleID)];
			std::string modString = modPrefix + m_modNames[info.modID];
			std::string functionString = functionPrefix + m_functionNames[info.funcID];

			std::string buttonText = typeString + '\n' + modString + '\n' + functionString;
			bool selected = (m_activeRule == info.ruleID);
			if (selected) {
				ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(0, 153, 153));
			}

			ImGui::PushID(info.ruleID);
			if (ImGui::Button(buttonText.c_str(), buttonDim)) {

				m_activeRule = info.ruleID;
			}
			// Listen for rmb click
			if (ImGui::BeginPopupContextItem("RuleOptions", 1)) {

				// Display menu for deleting the rule
				if (ImGui::MenuItem("Delete Rule")) {

					m_interface->m_deleteRule(info.ruleID);
					m_activeRule = INVALID_RULE_ID;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
			ImGui::PopID();

			if (selected) {
				ImGui::PopStyleColor();
			}
		}
		ImGui::EndChild();

		ImGui::EndChild();

		ImGui::PopStyleVar();
	}

	static RuleID lastRuleID = INVALID_RULE_ID;
	static uint16_t infoIndex = 0;

	void RuleEditor::DrawRuleDropdowns(float itemWidth) {

		if (m_activeRule != lastRuleID) {

			uint16_t numRules = static_cast<uint16_t>(m_ruleData.size());
			for (uint16_t i = 0; i < numRules; ++i) {

				if (m_activeRule == m_ruleData[i].ruleID) {
					infoIndex = i;
					break;
				}
			}

			lastRuleID = m_activeRule;
		}

		const RuleInfo& currentInfo = m_ruleData[infoIndex];
		RuleInfo infoCopy = currentInfo;

		ImGui::PushItemWidth(itemWidth);

		ImGui::BeginGroup();

		ImGui::PushID("Modifier");
		ImGui::LabelText("##Label", "Modifier");
		renderDropDown(infoCopy.modID, m_modNames, "##Drop");
		ImGui::PopID();

		ImGui::EndGroup();

		ImGui::SameLine();
		
		ImGui::BeginGroup();

		ImGui::PushID("Function");
		ImGui::LabelText("##Label", "Function");
		renderDropDown(infoCopy.funcID, m_functionNames, "##Drop");
		ImGui::PopID();

		ImGui::EndGroup();

		ImGui::PopItemWidth();

		if (infoCopy.modID != currentInfo.modID || infoCopy.funcID != currentInfo.funcID) {
			m_interface->m_setRuleInfo(infoCopy);
		}
	}

	const ImVec2 gridWindowPadding(40.0f, 40.0f);

	constexpr float scrollingScalar = 0.5f;
	constexpr float heightInterval = 0.1f;
	constexpr float widthInterval = 5.0f;

	void RuleEditor::DrawFunctionEditor() {

		const std::vector<RuleInfo>& infoVec = m_interface->m_getAllRuleData();
		for (const RuleInfo& info : infoVec) {
			
			if (info.ruleID == m_activeRule) {
				
				std::shared_ptr<Function> function = m_interface->m_getFunctionObject(info.funcID);

				m_editor.setEditable(info.funcID != DEFAULT_FUNCTION_ID);
				m_editor.Draw(function, hashIDToType(m_activeRule));

				break;
			}
		}
	}

	constexpr float gridMin = 0.0f;

	FunctionEditor::FunctionEditor() {

		m_gridRange = ImVec2(0.0f, 1.0f);
		m_gridDomain = ImVec2(0.0f, 50.0f);

		m_gridDensity = ImVec2(0.1f, 0.1f);
	}

	FunctionEditor::~FunctionEditor() {

	}

	void FunctionEditor::Draw(std::shared_ptr<Function> function, RuleType type) {

		UpdateWindowDimensions(function);

		DrawGridAxes(type);

		DrawGridLines();

		DrawLineSegments(function);
	}

	void FunctionEditor::UpdateWindowDimensions(std::shared_ptr<Function> function) {

		// Check if the window has resized since last frame
		ImVec2 pos = ImGui::GetWindowPos();
		
		// Recalculate the window dimensions
		m_topLeft = ImGui::GetCursorPos() + pos + gridWindowPadding;
		m_bottomRight = ImGui::GetWindowContentRegionMax() + pos - gridWindowPadding;
		m_windowWidth = m_bottomRight.x - m_topLeft.x;
		m_windowHeight = m_bottomRight.y - m_topLeft.y;

		// Draw the background rectangle for the graph
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddRectFilled(m_topLeft, m_bottomRight, backgroundColor);

		// Determine step size for the lines on the y-axis
		float intervalY = (m_gridRange.y - m_gridRange.x) * m_gridDensity.y;
		m_deltaY = (m_bottomRight.y - m_topLeft.y) * intervalY;

		// Determine step size for the lines on the x-axis
		m_deltaX = (m_bottomRight.x - m_topLeft.x) * m_gridDensity.x;

		ImVec2 cursorStash = ImGui::GetCursorPos();
		ImGui::SetCursorPos(m_topLeft - ImGui::GetWindowPos());
		ImGui::InvisibleButton("GraphBackground", ImVec2(m_windowWidth, m_windowHeight));
		ImGui::SetItemAllowOverlap();
		if (m_functionEditable && ImGui::BeginPopupContextItem("Vertex Options", 1) && !ImGui::IsMouseDragging(1)) {

			if (ImGui::IsMouseReleased(1)) {
				m_newVertex = ImGui::GetMousePos();
				std::cout << "MouseX: " << m_newVertex.x << " , TopLeft.x: " << m_topLeft.x << std::endl;
			}
			if (ImGui::MenuItem("Add Vertex")) {

				short vertX = static_cast<short>(std::round(windowDomainToFunctionDomain(m_newVertex.x)));
				float vertY = snapRangeValue(windowRangeToFunctionRange(m_newVertex.y));

				function->addVertex(vertX, vertY);
			}
			ImGui::EndPopup();
		}

		// Check key/mouse input that may change scaling or scrolling
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsWindowHovered()) {

			// Scroll the x axis left and right to get a different view of the graph
			if (ImGui::IsMouseDragging(1) && !ImGui::IsAnyItemActive()) {
				ImVec2 dragDelta = io.MouseDelta;
				m_scrollingOffset += dragDelta.x * scrollingScalar;

				// We need to update the domain since the scrolling amount has changed
				float deltaX = (m_bottomRight.x - m_topLeft.x) * m_gridDensity.x;
				float intervalX = (m_gridDomain.y - m_gridDomain.x) * m_gridDensity.x;

				// This is how many units we want to shift our starting position from
				int domainChange = static_cast<int>(-m_scrollingOffset / deltaX);
				if (m_scrollingOffset < 0.0f) {
					domainChange += 1;
				}

				// Shift amount
				float domainShift = domainChange * intervalX;
				float width = m_gridDomain.y - m_gridDomain.x;

				m_gridDomain.x = gridMin + domainShift;
				m_gridDomain.y = m_gridDomain.x + width;
			}

			// "Zoom" in and out on the x axis by removing or adding grid lines
			float wheelScroll = io.MouseWheel;
			if (wheelScroll != 0.0f) {
				if (io.KeysDown['Z']) {
					// We need a better mechanism for handling zooming
					//widthInterval += wheelScroll / 2;
				}
			}	
		}
	}

	void FunctionEditor::DrawGridLines() {

		// Get the draw list for the window
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Draw the grid lines running parallel to the x-axis first //

		// Determine step size for the lines on the y-axis
		float intervalY = (m_gridRange.y - m_gridRange.x) * m_gridDensity.y;

		float minX = m_topLeft.x;
		float maxX = m_bottomRight.x;

		float yCoord = m_gridRange.y;

		// Loop over each grid line. Note: the -5.0f in the condition fixes a text rendering bug on the bottom
		// of the graph
		for (float initY = m_topLeft.y; initY < m_bottomRight.y - 5.0f; initY += m_deltaY) {

			// Initialize a string stream object to enable easy label printing
			// Easier to recreate it every loop iteration than to reset it
			std::stringstream axisLabel;

			drawList->AddLine(ImVec2(minX, initY), ImVec2(maxX, initY), gridLineColor, 1.0f);
			axisLabel << std::fixed << std::setprecision(1) << yCoord;

			// On each line draw text to display the corresponding values
			drawList->AddText(ImVec2(m_yTextPos, initY), graphLabelColor, axisLabel.str().c_str());

			// Decrement counter variable for y text (moving down screen)
			yCoord -= intervalY;
		}

		// Next draw the lines parallel to the y-axis //
		// These do need to be offset by some scrolling amount
		float intervalX = (m_gridDomain.y - m_gridDomain.x) * m_gridDensity.x;

		float minY = m_topLeft.y;
		float maxY = m_bottomRight.y;

		float xCoord = m_gridDomain.x;
		m_firstX = xCoord;
		
		float initX = fmodf(m_scrollingOffset, m_deltaX);
		if (initX < 0.0f) { initX = m_deltaX + initX; }
		m_xoffset = initX;

		for (float xPos = 0.0f; (xPos = initX + m_topLeft.x) < m_bottomRight.x; initX += m_deltaX) {

			// Initialize a string stream object to enable easy label printing
			std::stringstream axisLabel;
			drawList->AddLine(ImVec2(xPos, minY), ImVec2(xPos, maxY), gridLineColor, 1.0f);
			axisLabel << xCoord;

			// On each line draw text to display corresponding values
			drawList->AddText(ImVec2(xPos, maxY + ImGui::GetTextLineHeight()), graphLabelColor, axisLabel.str().c_str());

			// Increment counter variable for x text
			xCoord += intervalX;
		}
		m_lastX = xCoord;
	}

	void FunctionEditor::DrawGridAxes(RuleType type) {

		ImDrawList* drawList = ImGui::GetWindowDrawList();

		// Helpers to keep function call lines easier to read
		ImVec2 bottomLeft(m_topLeft.x, m_bottomRight.y);
		ImVec2 topRight(m_bottomRight.x, m_topLeft.y);

		// Draw the x axis along the bottom of the region
		drawList->AddLine(bottomLeft, m_bottomRight, axisColor, 2.0f);

		float midPoint = m_bottomRight.x - bottomLeft.x - 100.0f;
		midPoint = midPoint / 2.0f;

		ImVec2 center(bottomLeft.x + midPoint, m_bottomRight.y + ImGui::GetTextLineHeight() * 2.25f);
		
		switch (type) {

		case ext_Pitch:
			drawList->AddText(center, graphLabelColor, "Pitch (MIDI note #)");
			break;

		case ext_Rhythm:
			drawList->AddText(center, graphLabelColor, "Rhythm (In 16th notes)");
			break;

		case ext_Interval:
			drawList->AddText(center, graphLabelColor, "Interval (Semitones)");
			break;

		case ext_Measure:
			drawList->AddText(center, graphLabelColor, "Rhythm (1 - 16) and Pitch (21 - 108)");
			break;

		case ext_Chord:
			drawList->AddText(center, graphLabelColor, "Interval between root and pitch (semitones)");
			break;

		// Other rule types...
		}


		// Setup data for rendering the y axis in the region at a scrolling offset
		ImVec2 yAxisTop(m_topLeft.x + m_scrollingOffset, m_topLeft.y);
		ImVec2 yAxisBottom(m_topLeft.x + m_scrollingOffset, bottomLeft.y);

		// Depending on the scrolling offset we might not render the y axis
		// scrolling offset will determine where the y axis value labels end up
		if (m_scrollingOffset > 0.0f && m_scrollingOffset < m_windowWidth) {

			drawList->AddLine(yAxisTop, yAxisBottom, axisColor, 2.0f);
			// Offset the text from the y axis by an arbitrary amount (it looks nice?)
			m_yTextPos = m_topLeft.x + m_scrollingOffset + (windowPadding.x / 2.0f);
		}
		else if (m_scrollingOffset <= 0.0f) {
			// Again, arbitrary offset to get the text along the left side of the graph
			m_yTextPos = m_topLeft.x - (windowPadding.x * 5.0f);
		} 
		else { // m_scrollingOffset >= m_windowWidth

			// More arbitrary offsets for aesthetics
			m_yTextPos = m_bottomRight.x + (windowPadding.x / 2.0f);
		}
	}

	constexpr float vertexRadius = 4.0f;
	const ImVec2 buttonOffset(-vertexRadius, -vertexRadius);

	void FunctionEditor::DrawLineSegments(std::shared_ptr<Function> function) {

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImVec2 cursorStash = ImGui::GetCursorPos();

		// Coordinates for line rendering
		ImVec2 minCoord;
		ImVec2 maxCoord;

		VertexIterator prevVert;
		VertexIterator currVert = function->begin();

		// Check if we can draw
		if (currVert == function->end()) { return; }

		drawList->ChannelsSplit(2);

		for (; currVert != function->end(); prevVert = currVert, ++currVert) {

			if (currVert == function->begin()) {
				prevVert = currVert;
			}

			float segmentMinX = functionDomainToWindowDomain(prevVert->_xPos);
			float segmentMaxX = functionDomainToWindowDomain(currVert->_xPos);

			// If the max is off the left side of the screen, skip this set and move on
			if (segmentMaxX < m_topLeft.x) { continue; }

			// If the min is off the right side of the screen, we're done
			if (segmentMinX > m_bottomRight.x) { break; }

			// At this point we're definitely drawing a segment this loop so start calculating
			// vertices in screen coordinates, we might have to redo some work in edge cases
			minCoord.x = functionDomainToWindowDomain(prevVert->_xPos);
			minCoord.y = functionRangeToWindowRange(prevVert->_yPos);

			maxCoord.x = functionDomainToWindowDomain(currVert->_xPos);
			maxCoord.y = functionRangeToWindowRange(currVert->_yPos);
			
			bool drawVertex = true;

			// If the min X is offscreen calculate the new coordinates for the min segment
			if (segmentMinX < m_topLeft.x) {

				float windowMin = minCoord.x;
				float windowMax = maxCoord.x;
				float ratio = (m_topLeft.x - windowMin) / (windowMax - windowMin);

				minCoord.x = m_topLeft.x;
				float newY = ratio * (currVert->_yPos - prevVert->_yPos) + prevVert->_yPos;
				minCoord.y = functionRangeToWindowRange(newY);
			}

			// If the max X is offscreen calculate the new coordinates of the max segment
			if (segmentMaxX > m_bottomRight.x) {

				float windowMin = minCoord.x;
				float windowMax = maxCoord.x;
				float ratio = (m_bottomRight.x - windowMin) / (windowMax - windowMin);

				maxCoord.x = m_bottomRight.x;
				float newY = ratio * (currVert->_yPos - prevVert->_yPos) + prevVert->_yPos;
				maxCoord.y = functionRangeToWindowRange(newY);

				drawVertex = false;
			}

			// We now have the two coordinates for the start and end of the line segment

			drawList->ChannelsSetCurrent(0); // Background channel
			
			// Draw the line segment
			drawList->AddLine(minCoord, maxCoord, segmentColor, 2.0f);

			drawList->ChannelsSetCurrent(1); // Foreground channel

			// Draw the vertex on the end of the line
			if (drawVertex) {

				drawList->AddCircleFilled(maxCoord, vertexRadius, vertexColor);

				// Invisible buttons for editing the vertex location
				ImGui::PushID(&(*currVert));

				ImGui::SetCursorPos(maxCoord - ImGui::GetWindowPos() + buttonOffset);
				ImGui::Button("##Vert", ImVec2(vertexRadius * 2.0f, vertexRadius * 2.0f));
				if (ImGui::IsItemActive()) {
					m_activeVertex = currVert->_vertID;
				}
				if (ImGui::BeginPopupContextItem("Vertex Edit", 1)) {

					if (ImGui::MenuItem("Delete Vertex")) {

						function->removeVertex(currVert->_xPos, currVert->_yPos);
						ImGui::SetCursorPos(cursorStash);

						ImGui::EndPopup();
						ImGui::PopID();

						break;
					}

					ImGui::EndPopup();
				}

				if (m_functionEditable && m_activeVertex == currVert->_vertID) {

					const ImVec2& mousePos = ImGui::GetMousePos();
					short vertNewX = static_cast<short>(std::round(windowDomainToFunctionDomain(mousePos.x)));
					float vertNewY = snapRangeValue(windowRangeToFunctionRange(mousePos.y));

					if (currVert->_xPos != vertNewX || currVert->_yPos != vertNewY) {

						std::cout << "Vertex ID: " << currVert->_vertID << std::endl;
						std::cout << "=======================================" << std::endl;
						std::cout << "Old X: " << currVert->_xPos << " New X: " << vertNewX << std::endl;
						std::cout << "Old Y: " << currVert->_yPos << " New Y: " << vertNewY << std::endl;
						std::cout << std::endl << std::endl;

						function->moveVertexYPos(currVert->_xPos, currVert->_yPos, vertNewY);
						function->moveVertexXPos(currVert->_xPos, vertNewX);
					}
				}

				ImGui::PopID();
			}
		}

		if (ImGui::IsMouseReleased(0)) {
			m_activeVertex = std::numeric_limits<uint16_t>::max();
		}

		drawList->ChannelsMerge();

		// Restore the draw cursor to its correct position
		ImGui::SetCursorPos(cursorStash);
	}

} // namespace Genetics