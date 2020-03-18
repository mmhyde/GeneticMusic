// Morgen Hyde

#include "Fitness/RuleManager.h"
#include "Fitness/RuleBuilder.h"
#include "Fitness/FunctionBuilder.h"

#include "FileIO/FitnessFiles.h"

#include <iostream>
#include <string>
#include <algorithm>

namespace Genetics {

#define WITHIN_RANGE(val, vector) (val < static_cast<uint16_t>(vector.size()))

	// Private Constructor to enforce our singleton pattern
	RuleManager::RuleManager()
		: m_ruleTable(RULES_PER_TYPE) {

		createDefaultFunction();
		m_modifierDatabase.push_back(nullptr); // Create a null modifier
		m_nextFuncID = 1;
	}

	RuleManager::~RuleManager() {

		shutdownManager();
	}


	// State Management
	void RuleManager::shutdownManager() {

		for (ModifierBase* mod : m_modifierDatabase) {

			delete mod;
			mod = nullptr;
		}

		m_functionDatabase.clear();
		m_modifierDatabase.clear();
		m_ruleInfoDatabase.clear();

		m_ruleTable.clearAllRules();
	}


	// Extractor creation functions

	// All at once
	void RuleManager::createAllExtractors(std::vector<ExtractorBase*>& extractorVec) {

		extractorVec.resize(ext_ExtractorCount);

		extractorVec[ext_Pitch] = new PitchExtractor(m_ruleTable.getRuleList<PitchRule>(ext_Pitch));
		extractorVec[ext_Rhythm]   = new RhythmExtractor(m_ruleTable.getRuleList<RhythmRule>(ext_Pitch));
	}

	// Add / Remove rules and rule components from the manager
	RuleID RuleManager::createRule(RuleType type, FunctionID functionID) {

		RuleID createdID = INVALID_RULE_ID;
		std::shared_ptr<Function> functionPtr;

		if (WITHIN_RANGE(functionID, m_functionDatabase) == false) {
			functionID = 0;
		}

		functionPtr = m_functionDatabase[functionID];

		createdID = m_ruleTable.constructRule(type, functionPtr);

		m_ruleInfoDatabase.emplace_back(createdID, functionID, NO_MODIFIER_ID);

		// Id is a handle to find type and index of the created element
		return createdID;
	}

	FunctionID RuleManager::createNewFunction(const std::string& functionName) {

		FunctionID index = m_nextFuncID++;
		std::string name = functionName;
		if (name == "function") {
			name += std::to_string(index);
		}
		m_functionDatabase.push_back(std::make_shared<Function>(name, index));
		return index;
	}

	void RuleManager::removeRule(const RuleID& ruleID) {

		m_ruleTable.destructRule(ruleID);
	}

	void RuleManager::removeFunction(const FunctionID& functionID) {

		if (WITHIN_RANGE(functionID, m_functionDatabase) == false) {
			std::cout << "Invalid function ID" << std::endl;
			return;
		}

		if (functionID == 0) {
			std::cout << "Cannot delete default function (ID == 0)" << std::endl;
			return;
		}

		// Search for every rule currently attempting to use this function
		for (RuleInfo& info : m_ruleInfoDatabase) {

			// If we find a match...
			if (info.funcID == functionID) {

				// Update the rule using the default function (0)
				setFunction(info.ruleID, 0);
			}
		}

		m_functionDatabase.erase(m_functionDatabase.begin() + functionID);
	}

	// Modify Rules

	void RuleManager::setModifier(const RuleID& ruleID, ModifierID modifierType) {

		if (WITHIN_RANGE(modifierType, m_modifierDatabase) == false) {
			std::cout << "Invalid modifier ID" << std::endl;
			return;
		}


	}

	void RuleManager::setFunction(const RuleID& ruleID, FunctionID functionType) {
		
		if (WITHIN_RANGE(functionType, m_functionDatabase) == false) {
			std::cout << "Invalid function ID" << std::endl;
			return;
		}

		std::shared_ptr<Function> function = m_functionDatabase[functionType];

		// Get the rule from the hash table
		RuleBase* ruleBase = m_ruleTable.getRule(ruleID);
		if (ruleBase) {

			// Set the new function
			ruleBase->m_evaluationFunction = function;
		}
	}

	void RuleManager::setRuleWeight(const RuleID& ruleID, float weight) {

		if (weight < 0.0f || weight > 1.0f) {
			std::cout << "Invalid weight assigned: " << weight << std::endl;
			return;
		}
	}

	// Modify through display struct
	void RuleManager::setRuleInfo(const RuleInfo& data) {

		for (RuleInfo& info : m_ruleInfoDatabase) {
			
			if (info.ruleID == data.ruleID) {

				info.funcID = data.funcID;
				info.modID = data.modID;
				break;
			}
		}
	}


	// Display Related Gettor functions
	const std::vector<std::string>& RuleManager::getRuleTypeList() {

		static std::vector<std::string> ruleTypeList;
		ruleTypeList.clear();
		ruleTypeList.emplace_back("Pitch Rule");
		ruleTypeList.emplace_back("Rhythm Rule");
		ruleTypeList.emplace_back("Interval Rule");
		ruleTypeList.emplace_back("Measure Rule");

		return ruleTypeList;
	}

	const std::vector<std::string>& RuleManager::getModifierNameList() {

		static std::vector<std::string> modifierNames;
		modifierNames.clear();
		modifierNames.reserve(m_modifierDatabase.size());
		for (const ModifierBase* mod : m_modifierDatabase) {
			if (mod == nullptr) {
				modifierNames.push_back("No Modifier");
			}
			else {
				//modifierNames.emplace_back(mod->getName());
			}
		}
		return modifierNames;
	}

	const std::vector<std::string>& RuleManager::getFunctionNameList() {
		
		static std::vector<std::string> functionNames;
		functionNames.clear();
		functionNames.reserve(m_functionDatabase.size());
		for (std::shared_ptr<Function>& func : m_functionDatabase) {
			functionNames.push_back(func->getName());
		}

		return functionNames;
	}


	const std::vector<RuleInfo>& RuleManager::getRuleData() {

		return m_ruleInfoDatabase;
	}

	// Get a modifiable handle to a specified Funtion object
	std::shared_ptr<Function> RuleManager::getFunctionHandle(FunctionID id) {

		if (id >= static_cast<FunctionID>(m_functionDatabase.size())) {
			return nullptr;
		}

		return m_functionDatabase[id];
	}

	// Serialization functions
	void RuleManager::setRootDirectory(const std::string& rootDirectory) {

		m_rootDirectory = rootDirectory;
	}

	bool RuleManager::importRules(const std::string& filepath) {

		// Create object to manager content importing
		XMLDeserializer deserialize(filepath);

		// Check we could open the file before making changes
		if (deserialize.isOpen() == false) {
			return false;
		}

		// Flush the contents of the manager (if any)
		m_functionDatabase.clear();
		m_nextFuncID = 1;
		m_ruleInfoDatabase.clear();

		m_ruleTable.clearAllRules();

		createDefaultFunction();

		while (deserialize.isNextTag("Rule")) {

			RuleInfo newInfo;

			deserialize.openRegion("Rule");

			deserialize.readTagData("RuleID", newInfo.ruleID);

			deserialize.readTagData("ModifierID", newInfo.modID);

			deserialize.readTagData("FunctionID", newInfo.funcID);

			deserialize.closeRegion("Rule");

			m_ruleInfoDatabase.push_back(newInfo);
		}

		while (deserialize.isNextTag("Function")) {

			deserialize.openRegion("Function");

			std::string functionName;
			deserialize.readTagData("Name", functionName);

			FunctionID functionID(-1);
			deserialize.readTagData("FunctionID", functionID);

			std::shared_ptr<Function> funcPtr = constructFunction(functionID, functionName);

			while (deserialize.isNextTag("Vertex")) {

				Vertex vertex(0, 0.0f);
				deserialize.readTagData("Vertex", vertex._xPos, vertex._yPos);

				funcPtr->addVertex(vertex);
			}

			deserialize.closeRegion("Function");
		}
		

		// Construct all the rules with new ID's now that everythings loaded from the file
		constructRules();

		return true;
	}

	bool RuleManager::exportRules(const std::string& filepath) {

		// Create object to manage content dumping
		XMLSerializer serialize(filepath);
		if (serialize.isOpen() == false) {
			return false;
		}

		// Write each rule info object to the file
		for (const RuleInfo& info : m_ruleInfoDatabase) {

			serialize.pushRegion("Rule");

			serialize.writeTagData("RuleID", info.ruleID);

			serialize.writeTagData("ModifierID", info.modID);

			serialize.writeTagData("FunctionID", info.funcID);

			serialize.popRegion();
		}

		// Write each function to the file
		for (std::shared_ptr<Function>& function : m_functionDatabase) {

			// Don't serialize the default function
			if (function->getFunctionID() == DEFAULT_FUNCTION_ID) {
				continue;
			}

			serialize.pushRegion("Function");

			serialize.writeTagData("Name", function->getName());

			serialize.writeTagData("FunctionID", function->getFunctionID());
			const std::vector<Vertex>& vertices = function->getVertices();
			for (const Vertex& vert : vertices) {

				serialize.writeTagData("Vertex", vert._xPos, vert._yPos);
			}

			serialize.popRegion();
		}

		return true;
	}


	// Private member functions

	void RuleManager::constructRules() {

		for (RuleInfo& info : m_ruleInfoDatabase) {

			// Get the appropriate type and function from the database entry
			RuleType type = hashIDToType(info.ruleID);
			std::shared_ptr<Function> function = getFunctionHandle(info.funcID);

			// Construct the rule and get a new ID for it
			RuleID newID = m_ruleTable.constructRule(type, function);
			
			// Assign the rule the new ID
			info.ruleID = newID;
		}
	}

	std::shared_ptr<Function> RuleManager::constructFunction(FunctionID id, const std::string& name) {

		m_functionDatabase.push_back(std::make_shared<Function>(name, id));
		return m_functionDatabase.back();
	}

	void RuleManager::createDefaultFunction() {

		FunctionID stash = m_nextFuncID;
		m_nextFuncID = 0;
		FunctionID temp = createNewFunction("Default Function"); // Create the default function
		m_nextFuncID = stash;

		std::shared_ptr<Function> function = getFunctionHandle(temp);

		function->addVertex(-10, 0.0f);
		function->addVertex(  0, 0.5f);
		function->addVertex( 10, 0.0f);
	}

} // namespace Genetics