#pragma once

#include <cstdint>

namespace Genetics {

	enum typesEnum {
		ext_Pitch = 0,
		ext_Rhythm,
		//ext_Interval,
		//ext_Measure,
		ext_ExtractorCount
	};

	typedef uint16_t RuleID;
	typedef uint8_t RuleType;

	typedef uint16_t FunctionID;
	typedef uint16_t ModifierID;

	class Function;
	class ModifierBase;

	constexpr uint32_t INVALID_RULE_ID = UINT16_MAX;
	constexpr ModifierID NO_MODIFIER_ID = 0;
	constexpr FunctionID DEFAULT_FUNCTION_ID = 0;
	constexpr uint16_t RULES_PER_TYPE = 16;

	struct RuleInfo {

		RuleInfo() 
			: ruleID(INVALID_RULE_ID), funcID(DEFAULT_FUNCTION_ID), modID(NO_MODIFIER_ID) {}

		RuleInfo(RuleID rule_id, FunctionID func_id, ModifierID mod_id)
			: ruleID(rule_id), funcID(func_id), modID(mod_id) {}

		RuleID     ruleID;
		FunctionID funcID;
		ModifierID modID;
	};

	typedef Function* FunctionPtr;

	__inline RuleType hashIDToType(RuleID id) {
		return static_cast<RuleType>(id >> 8);
	}

	__inline uint16_t hashIDToIndex(RuleID id) {
		return static_cast<uint16_t>(id & 0x00FF);
	}

	__inline RuleID hashTypeIndex(uint16_t type, uint16_t index) {
		return RuleID((type << 8) | (index & 0x00FF));
	}

} // namespace Genetics