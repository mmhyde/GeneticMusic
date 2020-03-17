// Morgen Hyde
#pragma once

#include "RuleList.h"
#include "RuleTypes.h"
#include "RuleBuilder.h"

#include <memory>
#include <vector>

// The intent here is to allocate a large array and use placement new to distribute
// rules of varying types into the table and use the x/y coordinates as an id
// so in memory (2D view) it may look like the following:

/*  The entire block of memory is allocated in one chunk (fixed number of rule types and number of rules per type)
	We can store the block as an array of RuleBase and then cast each row to the correct type of rule
	and send it to the various extractors so they have nice sequential memory accesses

	PitchRule   |pRule|pRule|pRule|nullPr|
	RhythmRule  |rRule|rRule|zero |nullPr|
	IntervalRule|iRule|iRule|iRule|nullPr|
	MeasureRule |mRule|zero |zero |nullPr|
*/

namespace Genetics {

	class RuleTable {

	public:
		RuleTable(const uint16_t maxRulesPerType);
		~RuleTable();

		RuleTable(const RuleTable& rhs) = delete;
		RuleTable& operator=(const RuleTable& rhs) = delete;

		RuleID constructRule(RuleType ruleType, std::shared_ptr<Function> ruleFunc, float weight = 1.0f/*, ModifierBase* ruleMod*/);
		void destructRule(const RuleID& ruleID);

		RuleBase* getRule(const RuleID& ruleID);

		void clearAllRules();

		template <class Rule>
		RuleList<Rule> getRuleList(RuleType type) const;

	private:

		__inline uint16_t firstElemOfType(uint16_t ruleType) const {
			
			return ruleType * RULES_PER_TYPE;
		}

		__inline uint16_t indexOfRule(RuleID id) const {
			
			RuleType type = hashIDToType(id);
			uint16_t relIndex = hashIDToIndex(id);

			return type * RULES_PER_TYPE + relIndex;
		}

		/* 
		// Another concept for managing rules, this way would avoid relying on a base class
		// Someday I'd like to run speed tests on this vs the base class pointer method and compare
		union RuleUnion {

			PitchRule pRule;
			RhythmRule rRule;

		private:
			// Constructor/Destructor marked private so only Union Manager can construct and destruct them
			RuleUnion() { };
			~RuleUnion() { };

			friend class RuleTable;
		};
		*/

		class RuleControl {

		public:
			RuleControl(RuleType ruleType, RuleBase* memoryAddr);
			~RuleControl();
			
			void construct(std::shared_ptr<Function> func, float weight);
			void destruct();

			RuleBase* getRule() const {
				return m_managedRule;
			}

			__inline bool isAvailable() const {
				return !m_constructed;
			}

		private:
			bool m_constructed;
			RuleBase* m_managedRule;
			const RuleType m_ruleType;
		};
		
		uint16_t m_ruleCounters[ext_ExtractorCount];
		std::vector<RuleControl> m_lookupTable;
		RuleBase* m_ruleStorage;

		const uint16_t m_maxRules;
	};

	namespace RuleConstructImpl {

		typedef void(*ConstructRule)(void* addr, std::shared_ptr<Function>, float);
		typedef void(*DestructRule)(void* addr);

		template <class Rule>
		void constructRule(void* addr, std::shared_ptr<Function> func, float weight) {

			new (addr) Rule(func, weight);
		}

		template <class Rule>
		void destructRule(void* addr) {

			reinterpret_cast<Rule*>(addr)->~Rule();
		}

	} // namespace RuleConstructImpl


	template <class Rule>
	RuleList<Rule> RuleTable::getRuleList(RuleType type) const {

		// Get offset into the stored rules array
		uint16_t arrayOffset = type * RULES_PER_TYPE;
		
		// Construct the object
		RuleList<Rule> list(reinterpret_cast<Rule*>(m_ruleStorage + arrayOffset), m_ruleCounters + type, RULES_PER_TYPE);
		
		// Return the list
		return list;
	}

} // namespace Genetics