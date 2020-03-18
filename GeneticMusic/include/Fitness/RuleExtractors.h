// Morgen Hyde
#pragma once

#include "RuleTypes.h"
#include "RuleList.h"

#include "Util.h"

#include <vector>

namespace Genetics {

	struct Phrase;

	class RuleBase;
	class PitchRule;
	class RhythmRule;
	class IntervalRule;
	class MeasureRule;

	class Modifier;
	class Function;

	class ExtractorBase {

	public:
		virtual ~ExtractorBase() {}
		
		virtual float process(Phrase* subject) const = 0;
		virtual void setRuleWeight(short id, float weight) = 0;
	};

	class PitchExtractor : public ExtractorBase {
	
	public:
		PitchExtractor(const RuleList<PitchRule> rules);
		~PitchExtractor();

		float process(Phrase* subject) const override;
		void setRuleWeight(short id, float weight) override;

	private:
		const RuleList<PitchRule> m_pitchRules;
	};

	class RhythmExtractor : public ExtractorBase {

	public:
		RhythmExtractor(const RuleList<RhythmRule> rules);
		~RhythmExtractor();

		float process(Phrase* subject) const override;
		void setRuleWeight(short id, float weight) override;
		
	private:
		const RuleList<RhythmRule> m_rhythmRules;
	};

	/*
	class IntervalExtractor : public ExtractorBase {

	public:
		IntervalExtractor();
		~IntervalExtractor();

		float process(Phrase* subject) override;
		void setRuleWeight(short id, float weight) override;

	private:
		const RuleList<IntervalRule> m_intervalRules;
	};

	class MeasureExtractor : public ExtractorBase {
		
	public:
		MeasureExtractor();
		~MeasureExtractor();

		float process(Phrase* subject) override;
		void setRuleWeight(short id, float weight) override;

	private:
		const RuleList<MeasureRule> m_measureRules;
	};

	*/
	// More extractors here...

} // namespace Genetics
