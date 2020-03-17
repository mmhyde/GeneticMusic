// Morgen Hyde
#pragma once

#include <string>
#include <fstream>

#include "FileIO/MIDIFiles.h"
#include "FileIO/WAVFiles.h"

namespace Genetics {

	struct Phrase;

	class FileManager {

	public:

		FileManager();
		FileManager(const std::string& rootDir);

		~FileManager();

		void setRootDirectory(const std::string& rootDir);
		const std::string& getRootDirectory() const;

		void writeToWav(Phrase* outputPhrase);
		void writeToMIDI(Phrase* outputPhrase, const std::string& filename);

		void readFromMIDI(Phrase* inputPhrase, const std::string& filename);

	private:

		MIDIHandler m_midiManager;

		std::string m_rootDir;
	};

} // namespace Genetics