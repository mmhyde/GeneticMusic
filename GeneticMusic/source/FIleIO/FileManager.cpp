
#include "FileIO/FileManager.h"

namespace Genetics {

	
	FileManager::FileManager() {

	}

	FileManager::FileManager(const std::string& rootDir) {

	}

	FileManager::~FileManager() {

	}

	void FileManager::setRootDirectory(const std::string& rootDir) {

	}

	const std::string& FileManager::getRootDirectory() const {

		return m_rootDir;
	}

	void FileManager::writeToWav(Phrase* outputPhrase) {

	}

	void FileManager::writeToMIDI(Phrase* outputPhrase, const std::string& filename) {

		m_midiManager.writeToMIDI(outputPhrase, m_rootDir + filename);

	}

	void FileManager::readFromMIDI(Phrase* inputPhrase, const std::string& filename) {

	}
	

} // namespace Genetics