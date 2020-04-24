// Morgen Hyde

#include "Graphics/UISystem.h"

// UI Elements
#include "Graphics/PianoRoll.h"
#include "Graphics/RuleEditor.h"

// Headers to help initialize individual UI elements
#include "Fitness/RuleManager.h"

// ImGui Helpers
#include "Graphics/ImGuiHelpers.h"

// ImGui and Rendering headers
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"

#include <iostream>
#include <fstream>

extern ImU32 whiteKey;
extern ImU32 blackKey;
extern ImU32 keyBorder;

extern ImU32 blackBackground;
extern ImU32 background;
extern ImU32 gridLines;

extern ImU32 timelineBackground;
extern ImU32 timelineColor;
extern ImU32 timelineNotchText;

extern ImU32 noteColor;
extern ImU32 chordColor;
extern ImU32 noteBorderColor;
extern ImU32 noteLineColor;

extern ImU32 ScrollBackground;
extern ImU32 ScrollTrough;
extern ImU32 ScrollBar;

// Editor Colors
extern ImU32 backgroundColor;
extern ImU32 axisColor;
extern ImU32 gridLineColor;
extern ImU32 vertexColor;
extern ImU32 segmentColor;

extern ImU32 graphLabelColor;

namespace Genetics {

	void error_callback(int error, const char* description) {
		std::cout << "GRAPHICS ERROR: " << error << " - " << description << std::endl;
	}

	UISystem::UISystem() {

		if (!glfwInit())
		{
			std::cout << "Failed to initialize glfw!" << std::endl;
			return;
		}

		glfwSetErrorCallback(error_callback);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

		m_window = glfwCreateWindow(1280, 720, "Genetic Music", nullptr, nullptr);
		if (!m_window)
		{
			std::cout << "Failed to create glfw window!" << std::endl;
			return;
		}

		glfwMakeContextCurrent(m_window);

		if (!gladLoadGL())
		{
			std::cout << "failed to initialize glad loader" << std::endl;
			return;
		}	
	}

	UISystem::~UISystem() {

		for (UIElement* elem : m_elements) {
			delete elem;
		}
	}

	void UISystem::initialize() {

		ImGui::CreateContext();

		ImGuiIO& ioObject = ImGui::GetIO();
		ioObject.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ioObject.IniFilename = NULL;

		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init("#version 430");

		

		m_windowClose = false;
	}

	void UISystem::shutdown() {

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();

		glfwDestroyWindow(m_window);
		glfwTerminate();

		m_window = nullptr;
	}

	void UISystem::startFrame() {

		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		pushAllStyleColors();
	}

	void UISystem::render() {

		startFrame();

		// Code from ImGui Docking example:
		static bool optional_fullscreen_persistant = true;
		bool optional_fullscreen = optional_fullscreen_persistant;
		static ImGuiDockNodeFlags dockingFlags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_AutoHideTabBar;
		dockingFlags |= (ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoWindowMenuButton);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar;

		if (optional_fullscreen) {

			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (dockingFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
			windowFlags |= ImGuiWindowFlags_NoBackground;
		}
		
		ImGui::Begin("Genetic Music", 0, windowFlags);

		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		if (!ImGui::DockBuilderGetNode(dockspace_id)) {

			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, dockingFlags | ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_NoWindowMenuButton);

			ImGuiID dockID_main = dockspace_id;
			ImGuiID dockID_piano = ImGui::DockBuilderSplitNode(dockID_main, ImGuiDir_Right, 0.75f, nullptr, &dockID_main);
			ImGuiID dockID_gen = ImGui::DockBuilderSplitNode(dockID_main, ImGuiDir_Up, 0.25f, nullptr, &dockID_main);
			ImGuiID dockID_music = ImGui::DockBuilderSplitNode(dockID_main, ImGuiDir_Up, 1.00f, nullptr, &dockID_main);

			ImGui::DockBuilderDockWindow("PianoRoll", dockID_piano);
			ImGuiDockNode* node = ImGui::DockBuilderGetNode(dockID_piano);
			node->LocalFlags |= (ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit);

			ImGui::DockBuilderDockWindow("GenerateWindow", dockID_gen);
			node = ImGui::DockBuilderGetNode(dockID_gen);
			node->LocalFlags |= (ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit);

			ImGui::DockBuilderDockWindow("MusicPlayer", dockID_music);
			node = ImGui::DockBuilderGetNode(dockID_music);
			node->LocalFlags |= (ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit);

			ImGui::DockBuilderDockWindow("PhraseList", dockID_main);
			node = ImGui::DockBuilderGetNode(dockID_main);
			node->LocalFlags |= (ImGuiDockNodeFlags_NoTabBar | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoDocking | ImGuiDockNodeFlags_NoSplit);

			ImGui::DockBuilderFinish(dockID_main);
		}
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockingFlags);

		static bool ruleBuilderWindow = false;
		static bool uiColorThemeWindow = false;

		ImGui::PopStyleVar(3);

		if (ImGui::BeginMenuBar()) {

			if (ImGui::BeginMenu("File")) {

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit")) {

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View")) {

				ImGui::MenuItem("Rule Builder", 0, &ruleBuilderWindow);
				ImGui::MenuItem("UI Theme", 0, &uiColorThemeWindow);

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGuiWindowFlags subWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

		ImGui::Begin("PianoRoll", 0, subWindowFlags);
		m_elements[0]->render();
		ImGui::End();

		ImGui::PopStyleVar();

		ImGui::Begin("GenerateWindow", 0, subWindowFlags);
		m_elements[4]->render();
		ImGui::End();

		ImGui::Begin("MusicPlayer", 0, subWindowFlags);
		m_elements[3]->render();
		ImGui::End();

		ImGui::Begin("PhraseList", 0, subWindowFlags);
		m_elements[2]->render();
		ImGui::End();

		ImGui::PopStyleVar(2);

		static ImVec2 builderWindowSize = ImGui::GetWindowContentRegionMax() * 0.80f;
		static ImVec2 colorWindowSize = ImGui::GetWindowContentRegionMax() * 0.80f;

		ImGui::End(); // Dockspace window end

		if (ruleBuilderWindow) {

			ImGui::SetNextWindowSize(builderWindowSize);

			ImGui::Begin("RuleBuilder", &ruleBuilderWindow);
			m_elements[1]->render();

			builderWindowSize = ImGui::GetWindowSize();
ImGui::End();
		}

		if (uiColorThemeWindow) {

			ImGui::SetNextWindowSize(colorWindowSize);

			ImGui::Begin("Color Window", &uiColorThemeWindow);
			renderUIColorPicker();

			colorWindowSize = ImGui::GetWindowSize();
			ImGui::End();
		}

		endFrame();
	}

	void UISystem::endFrame() {

		popAllStyleColors();

		ImGui::Render();
		glfwMakeContextCurrent(m_window);

		glfwGetFramebufferSize(m_window, &m_displayWidth, &m_displayHeight);

		glViewport(0, 0, m_displayWidth, m_displayHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(m_window);
		glfwSwapBuffers(m_window);

		if (glfwWindowShouldClose(m_window))
			m_windowClose = true;
	}

	// Helpers for UI color changing

	void UISystem::pushAllStyleColors() {

	}

	void UISystem::popAllStyleColors() {

	}

	void colorHelper(ImU32& color, const char* colorStr);
#define STRINGIFY(colorStr) #colorStr

	void UISystem::renderUIColorPicker() {

		// Color selection values:
		ImGuiStyle& style = ImGui::GetStyle();

		for (int16_t colorIdx = 0; colorIdx < ImGuiCol_COUNT; ++colorIdx) {

			const char* colorName = ImGui::GetStyleColorName(colorIdx);

			ImGui::PushID(colorIdx);
			ImGui::ColorEdit3("##color", (float*)&style.Colors[colorIdx]);

			ImGui::SameLine();
			ImGui::TextUnformatted(colorName);
			ImGui::PopID();
		}

		ImGui::Separator();

		colorHelper(whiteKey, STRINGIFY(whiteKey));
		colorHelper(blackKey, STRINGIFY(blackKey));
		colorHelper(keyBorder, STRINGIFY(keyBorder));

		colorHelper(blackBackground, STRINGIFY(blackBackground));
		colorHelper(background, STRINGIFY(background));
		colorHelper(gridLines, STRINGIFY(gridLines));

		colorHelper(timelineBackground, STRINGIFY(timelineBackground));
		colorHelper(timelineColor, STRINGIFY(timelineColor));
		colorHelper(timelineNotchText, STRINGIFY(timelineNotchText));

		colorHelper(noteColor, STRINGIFY(noteColor));
		colorHelper(chordColor, STRINGIFY(chordColor));
		colorHelper(noteBorderColor, STRINGIFY(noteBorderColor));
		colorHelper(noteLineColor, STRINGIFY(noteLineColor));

		colorHelper(ScrollBackground, STRINGIFY(ScrollBackground));
		colorHelper(ScrollTrough, STRINGIFY(ScrollTrough));
		colorHelper(ScrollBar, STRINGIFY(ScrollBar));

		colorHelper(backgroundColor, STRINGIFY(backgroundColor));
		colorHelper(axisColor, STRINGIFY(axisColor));
		colorHelper(gridLineColor, STRINGIFY(gridLineColor));
		colorHelper(vertexColor, STRINGIFY(vertexColor));
		colorHelper(segmentColor, STRINGIFY(segmentColor));

		colorHelper(graphLabelColor, STRINGIFY(graphLabelColor));

		if (ImGui::Button("Export")) {

			std::ofstream colorFile("Assets//ColorData.txt");
			if (!colorFile.is_open()) {
				std::cout << "Failed to open ColorData.txt" << std::endl;
				return;
			}
			
			colorFile.write(reinterpret_cast<char*>(&whiteKey), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&blackKey), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&keyBorder), sizeof(ImU32));

			colorFile.write(reinterpret_cast<char*>(&blackBackground), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&background), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&gridLines), sizeof(ImU32));

			colorFile.write(reinterpret_cast<char*>(&timelineBackground), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&timelineColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&timelineNotchText), sizeof(ImU32));

			colorFile.write(reinterpret_cast<char*>(&noteColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&chordColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&noteBorderColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&noteLineColor), sizeof(ImU32));

			colorFile.write(reinterpret_cast<char*>(&ScrollBackground), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&ScrollTrough), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&ScrollBar), sizeof(ImU32));

			colorFile.write(reinterpret_cast<char*>(&backgroundColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&axisColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&gridLineColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&vertexColor), sizeof(ImU32));
			colorFile.write(reinterpret_cast<char*>(&segmentColor), sizeof(ImU32));

			colorFile.write(reinterpret_cast<char*>(&graphLabelColor), sizeof(ImU32));
		}

		ImGui::SameLine();

		if (ImGui::Button("Import")) {

			std::ifstream colorFile("Assets//ColorData.txt");
			if (!colorFile.is_open()) {
				std::cout << "Failed to open ColorData.txt" << std::endl;
				return;
			}

			colorFile.read(reinterpret_cast<char*>(&whiteKey), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&blackKey), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&keyBorder), sizeof(ImU32));

			colorFile.read(reinterpret_cast<char*>(&blackBackground), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&background), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&gridLines), sizeof(ImU32));

			colorFile.read(reinterpret_cast<char*>(&timelineBackground), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&timelineColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&timelineNotchText), sizeof(ImU32));

			colorFile.read(reinterpret_cast<char*>(&noteColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&chordColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&noteBorderColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&noteLineColor), sizeof(ImU32));

			colorFile.read(reinterpret_cast<char*>(&ScrollBackground), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&ScrollTrough), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&ScrollBar), sizeof(ImU32));

			colorFile.read(reinterpret_cast<char*>(&backgroundColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&axisColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&gridLineColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&vertexColor), sizeof(ImU32));
			colorFile.read(reinterpret_cast<char*>(&segmentColor), sizeof(ImU32));

			colorFile.read(reinterpret_cast<char*>(&graphLabelColor), sizeof(ImU32));

		}
		
	}

	void colorHelper(ImU32& color, const char* colorStr) {

		ImGui::PushID(colorStr);
		
		ImVec4 colorVec4 = ImGui::ColorConvertU32ToFloat4(color);
		ImGui::ColorEdit4("##color", (float*)(&colorVec4), ImGuiColorEditFlags_NoAlpha);

		ImGui::SameLine();

		ImGui::TextUnformatted(colorStr);
		ImGui::PopID();

		color = ImGui::ColorConvertFloat4ToU32(colorVec4);
	}



} // namespace Genetics