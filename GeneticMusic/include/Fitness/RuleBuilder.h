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
		IntervalRule(std::shared_ptr<Function> evalFunction, float weight = 1.0f);

		IntervalRule(const IntervalRule& source);
		IntervalRule& operator=(const IntervalRule& rhs);

		~IntervalRule() {}

		float evaluate(uint8_t* intervalData, uint32_t count) const;
	};

	struct Measure {

		uint8_t* _pitches;
		uint8_t* _rhythms;
		uint32_t _numNotes;
	};

	class MeasureRule : public RuleBase {

	public:
		MeasureRule(std::shared_ptr<Function> evalFunction, float weight = 1.0f);

		MeasureRule(const MeasureRule& source);
		MeasureRule& operator=(const MeasureRule& rhs);

		~MeasureRule() {}

		float evaluate(Measure* measureData, uint32_t count) const;
	};



} // namespace Genetics