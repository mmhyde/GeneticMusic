#pragma once

#include <string>

namespace Genetics {

	class FuzzyBase {

	public:

		FuzzyBase();
		virtual ~FuzzyBase();

		virtual float Fuzzify(int value) = 0;
		virtual void SetEndpoints(int leftEnd = 0, int rightEnd = 0);
		virtual void SetCenterpoint(int center) = 0;
		virtual bool ValueIsInRange(int value);

	protected:
		FuzzyBase(const std::string& functionName);

		int m_leftEnd, m_rightEnd;
		std::string m_functionName;
	};

	class FuzzyTriangle : public FuzzyBase {

	public:
		FuzzyTriangle();
		virtual ~FuzzyTriangle();

		virtual float Fuzzify(int value) override;
		virtual void SetCenterpoint(int center = 0) override;

	protected:
		int m_centerPoint;
		
	};

	class FuzzyTrapezoid : public FuzzyBase {

	public:

		FuzzyTrapezoid();
		virtual ~FuzzyTrapezoid();

		virtual float Fuzzify(int value) override;
		virtual void SetCenterpoint(int center = 0) override;
		virtual void SetHalfWidth(int hWidth = 0);

	protected:

		int m_width;
		int m_centerpoint;
	};

	


} // namespace Genetics