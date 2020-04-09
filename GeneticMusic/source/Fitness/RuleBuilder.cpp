// Morgen Hyde
#include "Fitness/RuleBuilder.h"
#include "Fitness/FunctionBuilder.h"
#include "Fitness/RuleManager.h"

namespace Genetics {

	RuleBase::RuleBase(std::shared_ptr<Function> function, float weight)
		: m_evaluationFunction(function), m_ruleWeight(weight) {

	}

	RuleBase::RuleBase(const RuleBase& source)
		: m_evaluationFunction(source.m_evaluationFunction), m_modifier(source.m_modifier),
		  m_ruleWeight(source.m_ruleWeight) {

	}

	RuleBase& RuleBase::operator=(const RuleBase& rhs) {

		m_evaluationFunction = rhs.m_evaluationFunction;
		m_modifier = rhs.m_modifier;
		m_ruleWeight = rhs.m_ruleWeight;

		return *this;
	}

	void RuleBase::setNewWeight(float weight) {
		weight = (weight > 1.0f) ? 1.0f : ((weight < 0.0f) ? 0.0f : weight);
		m_ruleWeight = weight;
	}


	PitchRule::PitchRule(std::shared_ptr<Function> function, float weight)
		: RuleBase(function, weight) {

	}

	PitchRule::PitchRule(const PitchRule& source)
		: RuleBase(source) {

	}

	PitchRule& PitchRule::operator=(const PitchRule& rhs) {
		RuleBase::operator=(rhs);
		return *this;
	}

	float PitchRule::evaluate(char* pitchData, uint32_t count) const {

		float total = 0.0f;
		for (uint32_t i = 0; i < count; ++i) {

			total += (*m_evaluationFunction)(pitchData[i]);
		}

		return total / static_cast<float>(count);
	}


	RhythmRule::RhythmRule(std::shared_ptr<Function> evalFunction, float weight)
		: RuleBase(evalFunction, weight) {

	}

	RhythmRule::RhythmRule(const RhythmRule& source)
		: RuleBase(source) {

	}

	RhythmRule& RhythmRule::operator=(const RhythmRule& rhs) {

		RuleBase::operator=(rhs);
		return *this;
	}

	float RhythmRule::evaluate(char* rhythmData, uint32_t count) const {

		float total = 0.0f;
		for (uint32_t i = 0; i < count; ++i) {

			total += (*m_evaluationFunction)(rhythmData[i]);
		}

		return total / static_cast<float>(count);
	}


	IntervalRule::IntervalRule(std::shared_ptr<Function> evalFunction, float weight)
		: RuleBase(evalFunction, weight) {

	}

	IntervalRule::IntervalRule(const IntervalRule& source)
		: RuleBase(source) {

	}

	IntervalRule& IntervalRule::operator=(const IntervalRule& rhs) {

		RuleBase::operator=(rhs);
		return *this;
	}

	float IntervalRule::evaluate(uint8_t* intervalData, uint32_t count) const {

		float total = 0.0f;
		for (uint32_t i = 0; i < count; ++i) {

			total += (*m_evaluationFunction)(intervalData[i]);
		}

		return total / static_cast<float>(count);
	}


	MeasureRule::MeasureRule(std::shared_ptr<Function> evalFunction, float weight)
		: RuleBase(evalFunction, weight) {

	}

	MeasureRule::MeasureRule(const MeasureRule& source)
		: RuleBase(source) {

	}

	MeasureRule& MeasureRule::operator=(const MeasureRule& rhs) {

		RuleBase::operator=(rhs);
		return *this;
	}

	// This is kindof a weird one, my thought process here is that we make a weird function 
	// that has scores for rhythm values (1 - 16) since they don't overlap with any valid notes (21 - 108)
	float MeasureRule::evaluate(Measure* measureData, uint32_t count) const {

		float total = 0.0f;
		for (uint32_t i = 0; i < count; ++i) {

			const Measure& currentMeasure = measureData[i];
			for (uint16_t j = 0; j < currentMeasure._numNotes; ++j) {

				float pitchRes = (*m_evaluationFunction)(currentMeasure._pitches[j]);
				float rhythmRes = (*m_evaluationFunction)(currentMeasure._rhythms[j]);

				total += pitchRes * rhythmRes;
			}
		}

		return total / static_cast<float>(count);
	}

} // namespace Genetics