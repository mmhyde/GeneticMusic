#include "Fitness/FuzzyFunctions.h"

namespace Genetics {

	FuzzyBase::FuzzyBase() {}

	FuzzyBase::FuzzyBase(const std::string& functionName)
		: m_functionName(functionName) {}

	FuzzyBase::~FuzzyBase() {}

	void FuzzyBase::SetEndpoints(int leftEnd, int rightEnd) {

		m_leftEnd  = leftEnd;
		m_rightEnd = rightEnd;
	}

	bool FuzzyBase::ValueIsInRange(int value) {

		return value >= m_leftEnd && value <= m_rightEnd;
	}

	FuzzyTriangle::FuzzyTriangle()
		: FuzzyBase("TriangleFunction"), m_centerPoint(0) {}

	FuzzyTriangle::~FuzzyTriangle() {}

	float FuzzyTriangle::Fuzzify(int value) {

		if (!ValueIsInRange(value)) {
			return 0.0000001f;
		}

		float slope = 0.0f;

		if (value < m_centerPoint) {

			slope = (1.0f / static_cast<float>(m_centerPoint - m_leftEnd));
		}
		else if (value > m_centerPoint) {

			slope = (-1.0f / static_cast<float>(m_rightEnd - m_centerPoint));
		}
		
		return slope * static_cast<float>(value - m_centerPoint) + 1.0f;
	}

	void FuzzyTriangle::SetCenterpoint(int centerPoint) {

		m_centerPoint = centerPoint;
	}

	FuzzyTrapezoid::FuzzyTrapezoid()
		: FuzzyBase("FuzzyTrapezoid"), m_width(0), m_centerpoint(0) {}

	FuzzyTrapezoid::~FuzzyTrapezoid() {}

	float FuzzyTrapezoid::Fuzzify(int value) {

		if (value <= m_leftEnd) {

			return 0.0f;
		}
		else if (value < m_centerpoint - m_width) {

			return (static_cast<float>(value) / static_cast<float>((m_centerpoint - m_width) - m_leftEnd));
		}
		else if (value <= m_centerpoint + m_width) {

			return 1.0f;
		}
		else if (value < m_rightEnd) {

			return (static_cast<float>(-value) / static_cast<float>(m_rightEnd - (m_centerpoint + m_width)));
		}
		else {

			return 0.0f;
		}
	}

	void FuzzyTrapezoid::SetCenterpoint(int center) {

		m_centerpoint = center;
	}

	void FuzzyTrapezoid::SetHalfWidth(int width) {

		m_width = width;
	}


} // namespace Genetics