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

		static ImVec2 windowSize = ImGui::GetWindowContentRegionMax() * 0.80f;
		ImGui::End(); // Dockspace window end

		if (ruleBuilderWindow) {
			ImGui::SetNextWindowSize(windowSize);

			ImGui::Begin("RuleBuilder", &ruleBuilderWindow);
			m_elements[1]->render();

			windowSize = ImGui::GetWindowSize();
			ImGui::End();
		}

		endFrame();
	}

	void UISystem::endFrame() {

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

} // namespace Genetics