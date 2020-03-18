#pragma once

#include "Fitness/FunctionBuilder.h"
#include "Fitness/RuleBuilder.h"
#include "Fitness/RuleExtractors.h"
#include "Fitness/RuleTable.h"

#include "Fitness/RuleTypes.h"

#include <vector>
#include <unordered_map>
#include <queue>

namespace Genetics {

	class RuleManager {

	public:

		// Constructor / Destructor
		static RuleManager& getRuleManager() {
			static RuleManager manager;
			return manager;
		}

		RuleManager(const RuleManager& manager) = delete;
		RuleManager& operator=(const RuleManager& manager) = delete;

		~RuleManager();

		// State management
		void shutdownManager();

		// Extractor creation functions

		// Individuals
		template <class Extractor>
		ExtractorBase* createExtractorType();

		// All at once
		void createAllExtractors(std::vector<ExtractorBase*>& extractorVec);

		// Add / Remove rules and rule components from the manager
		RuleID createRule(RuleType type, FunctionID function = 0);
		FunctionID createNewFunction(const std::string& functionName = "default");

		void removeRule(const RuleID& ruleID);
		void removeFunction(const FunctionID& functionID);

		// Modify Rules
		void setModifier(const RuleID& ruleID, ModifierID modifierType);
		void setFunction(const RuleID& ruleID, FunctionID functionType);
		void setRuleWeight(const RuleID& ruleID, float weight = 1.0f);

		// Modify through display struct
		void setRuleInfo(const RuleInfo& data);

		// Display Related Gettor functions
		const std::vector<std::string>& getRuleTypeList();
		const std::vector<std::string>& getModifierNameList();
		const std::vector<std::string>& getFunctionNameList();

		const std::vector<RuleInfo>& getRuleData();

		// Get a modifiable handle to a certain Funtion object
		std::shared_ptr<Function> getFunctionHandle(FunctionID id);

		// Serialization functions
		void setRootDirectory(const std::string& rootDirectory);

		bool importRules(const std::string& filepath);
		bool exportRules(const std::string& filepath);

	private:
		RuleManager();

		// Helpers 
		void constructRules();
		std::shared_ptr<Function> constructFunction(FunctionID id, const std::string& name);

		void createDefaultFunction();

		// Variable declaration
		std::string m_rootDirectory;

		// Rule and rule component storage
		std::vector<std::shared_ptr<Function>> m_functionDatabase;
		FunctionID m_nextFuncID;

		std::vector<ModifierBase*> m_modifierDatabase;
		std::vector<RuleInfo> m_ruleInfoDatabase;

		RuleTable m_ruleTable;
	};

	// Individuals
	template <class Extractor>
	ExtractorBase* RuleManager::createExtractorType() {

	}

} // namespace Genetics