// Morgen Hyde
#pragma once

#include <memory>

namespace Genetics {

	class Function;
	class ModifierBase;
	class RuleManager;

	class RuleBase {

	public:
		RuleBase(std::shared_ptr<Function> evalFunction, float weight = 1.0f);
		RuleBase(const RuleBase& source);
		RuleBase& operator=(const RuleBase& rhs);

		~RuleBase() {}

		void setNewWeight(float weight);

	protected:
		std::shared_ptr<Function> m_evaluationFunction;
		ModifierBase* m_modifier;
		float m_ruleWeight;

		friend class RuleManager;
	};

	class PitchRule : public RuleBase {

	public:
		PitchRule(std::shared_ptr<Function> function, float weight = 1.0f);
		PitchRule(const PitchRule& source);
		PitchRule& operator=(const PitchRule& rhs);

		~PitchRule() {}

		float evaluate(char* pitchData, uint32_t count) const;
		
	};

	class RhythmRule : public RuleBase {

	public:
		RhythmRule(std::shared_ptr<Function> evalFunction, float weight = 1.0f);
		
		RhythmRule(const RhythmRule& source);
		RhythmRule& operator=(const RhythmRule& rhs);

		~RhythmRule() {}

		float evaluate(char* rhythmData, uint32_t count) const;
	};

	class IntervalRule : public RuleBase {

	public:

	private:

	};

	class MeasureRule : public RuleBase {

	public:

	private:

	};


	class ModifierBase {

	public:
		

	private:

	};

} // namespace Genetics