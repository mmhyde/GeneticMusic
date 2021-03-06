// Morgen Hyde
#pragma once

#include "RuleTypes.h"
#include "RuleList.h"

#include "Util.h"

#include <vector>

namespace Genetics {

	struct Phrase;

	// Forward declarations of function 
	struct Measure;

	class RuleBase;
	class PitchRule;
	class RhythmRule;
	class IntervalRule;
	class MeasureRule;
	class ChordRule;
	class ProgressionRule;

	class Modifier;
	class Function;

	class ExtractorBase {

	public:
		virtual ~ExtractorBase() {}
		
		virtual float process(Phrase* subject) const = 0;
		virtual uint16_t getNumRules() const = 0;
	};

	class PitchExtractor : public ExtractorBase {
	
	public:
		PitchExtractor(const RuleList<PitchRule> rules);
		~PitchExtractor();

		float process(Phrase* subject) const override;
		uint16_t getNumRules() const override { return m_pitchRules.getRuleCount(); }

	private:
		const RuleList<PitchRule> m_pitchRules;
		char* m_dataBuffer;
	};


	class RhythmExtractor : public ExtractorBase {

	public:
		RhythmExtractor(const RuleList<RhythmRule> rules);
		~RhythmExtractor();

		float process(Phrase* subject) const override;
		uint16_t getNumRules() const override { return m_rhythmRules.getRuleCount(); }

	private:
		const RuleList<RhythmRule> m_rhythmRules;
		char* m_dataBuffer;
	};

	
	class IntervalExtractor : public ExtractorBase {

	public:
		IntervalExtractor(const RuleList<IntervalRule> rules);
		~IntervalExtractor();

		float process(Phrase* subject) const override;
		uint16_t getNumRules() const override { return m_intervalRules.getRuleCount(); }

	private:
		const RuleList<IntervalRule> m_intervalRules;
		uint8_t* m_dataBuffer;
	};


	class MeasureExtractor : public ExtractorBase {
		
	public:
		MeasureExtractor(const RuleList<MeasureRule> rules);
		~MeasureExtractor();

		float process(Phrase* subject) const override;
		uint16_t getNumRules() const override { return m_measureRules.getRuleCount(); }

	private:
		const RuleList<MeasureRule> m_measureRules;
		Measure* m_dataBuffer;
	};


	// More extractors here...
	class ChordExtractor : public ExtractorBase {

	public:
		ChordExtractor(const RuleList<ChordRule> rules);
		~ChordExtractor();

		float process(Phrase* subject) const override;
		uint16_t getNumRules() const override { return m_chordRules.getRuleCount(); }

	private:
		const RuleList<ChordRule> m_chordRules;
		uint8_t* m_dataBuffer;
	};

	/*
	class ProgressionExtractor : public ExtractorBase {

	public:
		ProgressionExtractor(ProgressionRule rule);
		~ProgressionExtractor();

		float process(Phrase* subject) const override;
		uint16_t getNumRules() const override { return 1; }

	private:
		
	}
	*/

} // namespace Genetics
