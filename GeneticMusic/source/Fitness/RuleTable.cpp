// Morgen Hyde

#include "Fitness/RuleTable.h"


namespace Genetics {

	RuleTable::RuleTable(const uint16_t maxRulesPerType)
		: m_maxRules(maxRulesPerType * ext_ExtractorCount) {

		// Verify the rules stored in the table are correctly sized for the union
		static_assert(sizeof(RuleBase) == sizeof(PitchRule)
			&& sizeof(PitchRule) == sizeof(RhythmRule)
			//&& sizeof(PitchRule) == sizeof(IntervalRule) 
			//&& sizeof(PitchRule) == sizeof(MeasureRule)
			, "RuleTable requires all rules to be exactly the same size");

		// Allocate memory for the actual rules
		char* ruleMemory = new char[m_maxRules * sizeof(RuleBase)];
		m_ruleStorage = reinterpret_cast<RuleBase*>(ruleMemory);

		// Reserve space in the vector before constructing the manager structs
		m_lookupTable.reserve(m_maxRules);

		for (uint16_t i = 0; i < m_maxRules; ++i) {

			// Determine the current type
			RuleType currentType = static_cast<uint8_t>(i / maxRulesPerType);

			// Construct a new managing struct in the vector, giving it a piece of memory to manage
			m_lookupTable.emplace_back(currentType, m_ruleStorage + i);
		}

		// Ensure counters are all set to 0
		std::memset(m_ruleCounters, 0, sizeof(m_ruleCounters));
	}

	RuleTable::~RuleTable() {

		char* ruleMemory = reinterpret_cast<char*>(m_ruleStorage);
		delete[] ruleMemory;
		m_ruleStorage = nullptr;
	}

	RuleID RuleTable::constructRule(RuleType type, std::shared_ptr<Function> ruleFunc, float weight) {

		// Default to invalid
		RuleID constructedID = INVALID_RULE_ID;

		// Find location of next available slot for a rule
		uint16_t numRules = m_ruleCounters[type];
		if (numRules == RULES_PER_TYPE) {

			// No space remaining, return a failure
			return constructedID;
		}
		uint16_t availableIndex = type * RULES_PER_TYPE + numRules;

		// Construct an ID using the type and relative index of the rule
		constructedID = hashTypeIndex(type, numRules);

		// Construct the rule
		m_lookupTable[availableIndex].construct(ruleFunc, weight);

		// Increment the appropriate counter
		m_ruleCounters[type] += 1;

		// Return the id
		return constructedID;
	}

	void RuleTable::destructRule(const RuleID& ruleID) {

		// Convert ID to index
		uint16_t index = indexOfRule(ruleID);

		// Get the controller for the object and check validity
		RuleControl& controller = m_lookupTable[index];
		if (!controller.isAvailable()) {

			// Destroy the stored rule
			controller.destruct();

			// Decrement the appropriate counter
			m_ruleCounters[hashIDToType(ruleID)] -= 1;
		}
	}

	RuleBase* RuleTable::getRule(const RuleID& ruleID) {

		// Convert ID to index
		uint16_t index = indexOfRule(ruleID);
		RuleBase* rule = nullptr;

		// Get the controller for the object and check validity
		RuleControl& controller = m_lookupTable[index];
		if (!controller.isAvailable()) {

			rule = controller.getRule();
		}

		return rule;
	}

	void RuleTable::clearAllRules() {

		// Loop over all rule control blocks
		for (RuleControl& controlBlock : m_lookupTable) {

			// If it's occupied by a rule, destroy the rule
			if (!controlBlock.isAvailable()) {

				controlBlock.destruct();
			}
		}

		// Zero out all the rule counters
		std::memset(m_ruleCounters, 0, sizeof(m_ruleCounters));
	}

	// Array to hold methods for constructing each type of rule
	RuleConstructImpl::ConstructRule ruleConstructors[] =
	{
		RuleConstructImpl::constructRule<PitchRule>,
		RuleConstructImpl::constructRule<RhythmRule>
		//RuleConstructImpl::constructRule<IntervalRule>,
		//RuleConstructImpl::constructRule<MeasureRule>
	};

	// Array to hold methods for destructing each type of rule
	RuleConstructImpl::DestructRule ruleDestructors[] =
	{
		RuleConstructImpl::destructRule<PitchRule>,
		RuleConstructImpl::destructRule<RhythmRule>
		//RuleConstructImpl::destructRule<IntervalRule>,
		//RuleConstructImpl::destructRule<MeasureRule>
	};

	RuleTable::RuleControl::RuleControl(RuleType ruleType, RuleBase* memoryAddr)
		: m_ruleType(ruleType), m_constructed(false), m_managedRule(memoryAddr) {
	}

	RuleTable::RuleControl::~RuleControl() {

		// Call the appropriate destructor for the element
		if (m_constructed) {
			ruleDestructors[m_ruleType](m_managedRule);
		}
	}

	void RuleTable::RuleControl::construct(std::shared_ptr<Function> func, float weight) {

		// Construct the object, dispatch using stored type
		if (!m_constructed) {
			ruleConstructors[m_ruleType](m_managedRule, func, weight);
		}
	}

	void RuleTable::RuleControl::destruct() {

		// Call the destructor on the object, using stored type as index
		if (m_constructed) {
			ruleDestructors[m_ruleType](m_managedRule);
		}
	}

} // namespace Genetics