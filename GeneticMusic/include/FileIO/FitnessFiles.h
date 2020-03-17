// Morgen Hyde
#pragma once

#include <string>
#include <sstream>
#include <fstream>


#include <stack>


namespace Genetics {

	__inline std::string makeOpeningTag(const std::string& tag) {

		return "<" + tag + ">";
	}

	__inline std::string makeClosingTag(const std::string& tag) {

		return "</" + tag + ">";
	}

	class XMLDeserializer {

	public:
		XMLDeserializer(const std::string& fileLocation);
		~XMLDeserializer();

		XMLDeserializer(const XMLDeserializer& rhs) = delete;
		XMLDeserializer& operator=(const XMLDeserializer& rhs) = delete;

		// Worker functions:
		bool isNextTag(const std::string& tag);

		bool openRegion(const std::string& tag);
		bool closeRegion(const std::string& tag);

		template <typename... Args>
		void readTagData(const std::string& tag, Args&&... others);

		// Class information gettors:
		__inline bool isOpen() { return m_isOpen; }

	private:
		template <typename T>
		struct reader {
			void operator()(const std::string& fileData, T&& data) {
				
				std::istringstream(fileData) >> data;
			}
		};

		template <>
		struct reader<std::string&> {
			void operator()(const std::string& fileData, std::string& data) {

				data.resize(fileData.size());
				fileData.copy(&(data.front()), fileData.size());

				// Find and replace all underscores with spaces
				size_t underscorePos = data.find('_');
				while (underscorePos != std::string::npos) {

					data[underscorePos] = ' ';
					underscorePos = data.find('_', underscorePos + 1);
				}
			}
		};

		template <typename T, typename... Args>
		void readData(T&& data, Args&&... others);

		template <typename T>
		void readData(T&& data);

		std::stack<std::string> m_tagStack;

		const std::string m_fileLocation;
		std::ifstream m_inputFile;
		bool m_isOpen;
	};

	template <typename... Args>
	void XMLDeserializer::readTagData(const std::string& tag, Args&&... others) {

		// Save stream position from the file in case of an error
		std::streampos pos = m_inputFile.tellg();

		// Read opening tag from the file
		std::string readTag;
		m_inputFile >> readTag;

		// Verify tags
		if (readTag != makeOpeningTag(tag)) {

			// Tag mismatch, stop reading, reset stream position
			m_inputFile.seekg(pos);
			return;
		}

		// Read next file elements into the provided references
		readData(std::forward<Args>(others)...);

		// Read closing tag from the file
		m_inputFile >> readTag;

		// Verify tags
		if (readTag != makeClosingTag(tag)) {

			// Tag mismatch, stop reading, reset stream position
			m_inputFile.seekg(pos);
			return;
		}
	}

	template <typename T, typename... Args>
	void XMLDeserializer::readData(T&& data, Args&&... others) {

		std::string value;
		m_inputFile >> value;
		
		reader<T> temp;
		temp(value, data);

		readData(std::forward<Args>(others)...);
	}

	template <typename T>
	void XMLDeserializer::readData(T&& data) {

		std::string value;
		m_inputFile >> value;
		
		reader<T> temp;
		temp(value, data);
	}

	class XMLSerializer {

	public:
		XMLSerializer(const std::string& fileLocation);
		~XMLSerializer();

		XMLSerializer(const XMLSerializer& rhs) = delete;
		XMLSerializer& operator=(const XMLSerializer& rhs) = delete;

		// Worker functions:

		void pushRegion(const std::string& tag);
		void popRegion();

		template <typename... Args>
		void writeTagData(const std::string& tag, Args... others);

		// Class information gettors:
		__inline bool isOpen() { return m_isOpen; }

	private:
		template <typename T, typename... Args>
		void writeData(T data, Args... others);

		template <typename T>
		void writeData(T data);

		template <typename T>
		struct writer {
			std::string operator()(T data) {

				return std::to_string(data);
			}
		};

		template <>
		struct writer<std::string> {
			std::string operator()(const std::string& data) {
				
				std::string newString(data);
				size_t spacePos = newString.find(' ');
				while (spacePos != std::string::npos) {

					newString[spacePos] = '_';
					spacePos = newString.find(' ');
				}

				return newString;
			}
		};

		std::stack<std::string> m_tagStack;
		std::string m_indent;

		const std::string m_fileLocation;
		std::ofstream m_outputFile;
		bool m_isOpen;
	};

	template <typename... Args>
	void XMLSerializer::writeTagData(const std::string& tag, Args... others) {

		// Write tag to the file
		const std::string& opening = makeOpeningTag(tag);
		m_outputFile << m_indent << opening << std::endl;

		// Dispatch data recursively
		writeData(others...);

		// Write closing tag
		const std::string& closing = makeClosingTag(tag);
		m_outputFile << m_indent << closing << std::endl;
	}

	template <typename T, typename... Args>
	void XMLSerializer::writeData(T data, Args... others) {

		// Write single element to the file
		writer<T> temp;
		const std::string& elem = temp(data);

		m_outputFile << m_indent << elem << std::endl;

		// Dispatch remaining recursively
		writeData(others...);
	}

	template <typename T>
	void XMLSerializer::writeData(T data) {

		// Write the element to the file
		writer<T> temp;
		const std::string& elem = temp(data);

		m_outputFile << m_indent << elem << std::endl;
	}


} // namespace Genetics