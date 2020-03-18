#pragma once

#include "UIElements.h"
#include "GenericFunctor.h"

#include "Fitness/RuleTypes.h"

#include "imgui/imgui.h"

#include <vector>

namespace Genetics {

	// What this class does:
	// + Displays information about all the rules being used in determining fitness
	// + Displays graph of the function for the currently selected rule
	// + Allows editing of the currently selected rule, specifically: type, modifier, and function

	struct RuleManagerInterface;

	class FunctionEditor {

	public:
		FunctionEditor();
		~FunctionEditor();

		void Draw(std::shared_ptr<Function> function, RuleType type);

		void setEditable(bool functionEditable) {
			m_functionEditable = functionEditable;
		}

	private:
		// Verify window dimensions haven't changed since last tick,
		// if they have recalculate internal variables
		void UpdateWindowDimensions(std::shared_ptr<Function> function);

		void DrawGridLines();

		void DrawGridAxes(RuleType type);

		void DrawLineSegments(std::shared_ptr<Function> function);

		__inline float functionDomainToWindowDomain(float value) const {

			float absolute = (value * m_windowWidth) / (m_gridDomain.y - m_gridDomain.x);
			return absolute + m_topLeft.x + m_scrollingOffset;
		}

		__inline float windowDomainToFunctionDomain(float xCoord) const {

			float winPos = xCoord - m_topLeft.x - m_scrollingOffset;
			return winPos * (m_gridDomain.y - m_gridDomain.x) / m_windowWidth;
		}

		__inline float functionRangeToWindowRange(float value) const {
			
			return (1.0f - value) * m_windowHeight + m_topLeft.y;
		}

		__inline float windowRangeToFunctionRange(float yCoord) const {

			return 1.0f - (yCoord - m_topLeft.y) / m_windowHeight;
		}

		__inline float snapRangeValue(float value) const {

			float temp = value * 10.0f;
			float output = std::round(temp);
			return output / 10.0f;
		}

		__inline float gridWidth() const {

			return 0.0f;
		}

		ImVec2 m_gridRange;
		ImVec2 m_gridDomain;

		ImVec2 m_gridDensity;

		float m_deltaY;
		float m_deltaX;

		// Coordinates for the corner of the function graph
		ImVec2 m_topLeft;
		ImVec2 m_bottomRight;

		float m_windowWidth;
		float m_windowHeight;

		// Y Axis related variables
		float m_yTextPos;

		// Line segment variables
		float m_firstX;
		float m_lastX;
		
		float m_xoffset;
		uint32_t m_activeVertex;

		ImVec2 m_newVertex;

		bool m_functionEditable;

		// Scrolling information
		float m_scrollingOffset;
	};

	class RuleEditor : public UIElement {

	public:
		RuleEditor(std::unique_ptr<RuleManagerInterface> managerInterface);
		~RuleEditor();

		void render() override;

	private:
		void DrawRuleList();
		void DrawRuleDropdowns(float itemWidth);

		void DrawFunctionButtons(float buttonWidth);
		void DrawFunctionEditor();

		RuleID m_activeRule;

		std::unique_ptr<RuleManagerInterface> m_interface;

		const std::vector<RuleInfo>& m_ruleData;

		const std::vector<std::string>& m_ruleTypeNames;
		const std::vector<std::string>& m_modNames;
		const std::vector<std::string>& m_functionNames;

		FunctionEditor m_editor;
	};



} // namespace Genetics