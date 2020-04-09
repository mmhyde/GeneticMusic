// Morgen Hyde

#pragma once

#include <cstdint>

namespace Genetics {

	template <class Rule>
	class RuleList {

	public:
		RuleList(Rule* m_ruleArray, const uint16_t* ruleCounter, const uint16_t& maxRules);
		~RuleList();

		RuleList(const RuleList& rhs);

		template <typename InputType>
		float evaluateAll(InputType type, uint32_t inputLen) const;

		uint16_t getRuleCount() const { 
			return *m_numRules; 
		}

		uint16_t getRuleCapacity() const {
			return m_maxRules;
		}

	private:
		Rule* m_ruleType;

		const uint16_t* m_numRules;
		const uint16_t m_maxRules;
	};

	template <class Rule>
	RuleList<Rule>::RuleList(Rule* m_ruleType, const uint16_t* ruleCounter, const uint16_t& maxRules)
		: m_ruleType(m_ruleType), m_numRules(ruleCounter), m_maxRules(maxRules) {
	}

	template <class Rule>
	RuleList<Rule>::RuleList(const RuleList& rhs)
		: m_ruleType(rhs.m_ruleType), m_numRules(rhs.m_numRules), m_maxRules(rhs.m_maxRules) {

	}

	template <class Rule>
	RuleList<Rule>::~RuleList() {
		// Don't call delete on the array of rules, we don't own it
	}

	template<class Rule>
	template<typename InputType>
	float RuleList<Rule>::evaluateAll(InputType type, uint32_t inputLen) const {

		if (*m_numRules == 0 || m_ruleType == nullptr) { return 0.0f; }

		float total = 0.0f;
		for (uint16_t i = 0; i < *m_numRules; ++i) {

			total = m_ruleType[i].evaluate(type, inputLen);
		}

		return total;
	}

} // namespace Genetics
