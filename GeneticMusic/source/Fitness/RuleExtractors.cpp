#include "Fitness/RuleExtractors.h"
#include "Fitness/RuleBuilder.h"

#include "Phrase.h"

namespace Genetics {


	PitchExtractor::PitchExtractor(const RuleList<PitchRule> rules)
		: m_pitchRules(rules) {

	}

	PitchExtractor::~PitchExtractor() {

	}

	float PitchExtractor::process(Phrase* subject) const {

		float total = 0.0f;
		char* pitchArray = new char[subject->_melodicNotes];
		for (uint32_t i = 0, note = 0; i < subject->_melodicNotes; ++i) {

			pitchArray[i] = subject->_melodicData[note];
			note += subject->_melodicRhythm[note];
		}

		return m_pitchRules.evaluateAll(pitchArray, subject->_melodicNotes);
	}

	void PitchExtractor::setRuleWeight(short id, float weight) {

	}

	RhythmExtractor::RhythmExtractor(const RuleList<RhythmRule> rules)
		: m_rhythmRules(rules) {

	}

	RhythmExtractor::~RhythmExtractor() {

	}

	float RhythmExtractor::process(Phrase* subject) const {


		return 0.0f;
	}

	void RhythmExtractor::setRuleWeight(short id, float weight) {

	}

/*
	IntervalExtractor::IntervalExtractor() {

	}

	IntervalExtractor::~IntervalExtractor() {

	}

	float IntervalExtractor::process(Phrase* subject) {

	}

	short IntervalExtractor::addRule(RuleBase* newRule, float weight) {

	}

	void IntervalExtractor::setRuleModifier(short id, Modifier* modifier) {

	}

	void IntervalExtractor::setRuleFunction(short id, FunctionBase* function) {

	}

	void IntervalExtractor::setRuleWeight(short id, float weight) {

	}



	MeasureExtractor::MeasureExtractor() {

	}

	MeasureExtractor::~MeasureExtractor() {

	}

	float MeasureExtractor::process(Phrase* subject)  {

	}

	short MeasureExtractor::addRule(RuleBase* newRule, float weight) {

	}

	void MeasureExtractor::setRuleModifier(short id, Modifier* modifier) {

	}

	void MeasureExtractor::setRuleFunction(short id, FunctionBase* function) {

	}

	void MeasureExtractor::setRuleWeight(short id, float weight) {

	}
*/

} // namespace Genetics