// Morgen Hyde

#include "FileIO/FitnessFiles.h"

#ifdef _DEBUG
#include <iostream>
#endif

namespace Genetics {

	const char xmlHeader[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";

	XMLDeserializer::XMLDeserializer(const std::string& fileLocation)
		: m_fileLocation(fileLocation), m_inputFile(fileLocation) {

		m_isOpen = m_inputFile.is_open();
	}

	XMLDeserializer::~XMLDeserializer() {
		
		if (!m_tagStack.empty()) {
#ifdef _DEBUG
			std::cout << "Open/Close Region mismatch!" << std::endl;
#endif
		}
	}

	bool XMLDeserializer::isNextTag(const std::string& tag) {
		
		// Save file position
		std::streampos position = m_inputFile.tellg();

		// Read the next line from the file
		std::string nextTag;
		m_inputFile >> nextTag;

		// Reset file position
		m_inputFile.seekg(position);

		// Compare strings
		return !(nextTag.find(tag) == std::string::npos);
	}

	bool XMLDeserializer::openRegion(const std::string& tag) {

		// Save file position
		std::streampos pos = m_inputFile.tellg();

		// Read the next line from the file
		std::string region;
		m_inputFile >> region;

		// Check if the expected tag matches the actual one
		const std::string& openTag = makeOpeningTag(tag);
		if (openTag == region) {

			m_tagStack.push(tag);
			return true;
		}
		
		// Expected tag does not match
		m_inputFile.seekg(pos);

#ifdef _DEBUG
		std::cout << "Tag mismatch found, expected: ";
		std::cout << openTag << ", found: " << region;
		std::cout << std::endl;
#endif
		return false;
	}

	bool XMLDeserializer::closeRegion(const std::string& tag) {

		if (m_tagStack.empty()) {

#ifdef _DEBUG
			std::cout << "Attempted to close unopened region" << std::endl;
#endif
			return false;
		}

		const std::string& topTag = m_tagStack.top();
		if (topTag != tag) {

#ifdef _DEBUG
			std::cout << "Attempted to close incorrect region!"; 
			std::cout << "Expected: " << topTag << ", attempted: " << tag;
			std::cout << std::endl;
#endif
			return false;
		}

		// Save file stream position
		std::streampos pos = m_inputFile.tellg();

		// Read next line from the file
		std::string regionEnd;
		m_inputFile >> regionEnd;

		const std::string& endTag = makeClosingTag(tag);
		if (endTag == regionEnd) {
			
			// Pop the tag off the stack
			m_tagStack.pop();
			return true;
		}

#ifdef _DEBUG
		std::cout << "Tag mismatch found, expected: ";
		std::cout << endTag << ", found: " << regionEnd;
		std::cout << std::endl;
#endif
		return false;
	}


	XMLSerializer::XMLSerializer(const std::string& fileLocation)
		: m_fileLocation(fileLocation), m_outputFile(fileLocation) {

		m_isOpen = m_outputFile.is_open();  
	}

	XMLSerializer::~XMLSerializer() {

	}

	void XMLSerializer::pushRegion(const std::string& tag) {

		const std::string& opening = makeOpeningTag(tag);

		m_outputFile << m_indent << opening << std::endl;

		// Push the tag onto the stack for later
		m_tagStack.push(tag);
		m_indent.push_back('\t');
	}

	void XMLSerializer::popRegion() {

		const std::string& closing = makeClosingTag(m_tagStack.top());
		
		// Pop the tag off the stack
		m_tagStack.pop();
		m_indent.pop_back();

		m_outputFile << m_indent << closing << std::endl;
	}

} // namespace Genetics