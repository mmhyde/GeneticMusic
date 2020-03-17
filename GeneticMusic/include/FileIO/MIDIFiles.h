// Morgen Hyde
#pragma once

#include <string>
#include <unordered_map>
#include <queue>

namespace Genetics {

	struct Phrase;
	struct MIDIEvent;

	class MIDIHandler {

	public:

		MIDIHandler();
		~MIDIHandler();

		void setRootDirectory(const std::string& rootDir);

		void writeToMIDI(Phrase* outputPhrase, const std::string& filename);
		void readFromMIDI(Phrase* inputPhrase, const std::string& filename);

	private:

		unsigned int ticksPerQuarterToArrayIndex(unsigned int ticks, unsigned int division);
		unsigned int ticksPerSecondToTicksPerQuarter(unsigned int ticks);

		unsigned int arrayIndexToTicksPerQuarter(unsigned int arrayIndex, unsigned int division);

		void dumpQueueToPhrase(Phrase*);
		void dumpPhraseToQueue(Phrase*);

		struct StatusHandler {

			void(*readFunction)(std::ifstream&, MIDIEvent&, const unsigned char&);
			void(*writeFunction)(std::ofstream&, const MIDIEvent&, const unsigned char&);

			unsigned char m_statusByte; // 0x8_, 0x9_, 0xA_, etc.
		};

		struct MIDIComparison {
			bool operator()(const MIDIEvent&, const MIDIEvent&) const;
		};

		std::vector<MIDIEvent> m_eventQueue;
		std::unordered_map<unsigned char, StatusHandler> m_statusHandler;
		
		std::string m_rootDirectory;
	};

} // namespace Genetics
