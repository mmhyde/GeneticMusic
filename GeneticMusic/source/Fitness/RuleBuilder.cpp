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




} // namespace Genetics