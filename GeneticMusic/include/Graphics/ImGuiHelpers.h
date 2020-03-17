#include "imgui/imgui.h"

#pragma once

namespace Genetics {

	constexpr float comparison_epsilon = 0.0001f;

	static inline ImVec2 operator+(const ImVec2& left, const ImVec2& right) { return ImVec2(left.x + right.x, left.y + right.y); }
	static inline ImVec2 operator-(const ImVec2& left, const ImVec2& right) { return ImVec2(left.x - right.x, left.y - right.y); }
	static inline ImVec2 operator*(const ImVec2& left, float scalar) { return ImVec2(left.x * scalar, left.y * scalar); }

	static inline bool operator==(const ImVec2& left, const ImVec2& right) {
		if (left.x > right.x - comparison_epsilon && left.x < right.x + comparison_epsilon) {
			if (left.y > right.y - comparison_epsilon && left.y < right.y + comparison_epsilon) {
				return true;
			}
		}
		return false;
	}

} // namespace Genetics