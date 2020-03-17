
#include "Phrase.h"
#include "FileIO/MIDIFiles.h"
#include "FileIO/FileDefinitions.h"

#include <fstream>
#include <iostream>
#include <algorithm>

namespace Genetics {

	struct MIDIEvent {
		// stores note position as an integral number of smallestSubdivison
		int m_subDivisionPosition;
		
		// We only really care about note on and off messages, so we'll ignore almost all others
		unsigned char m_statusByte;
		unsigned char m_firstByte;
		unsigned char m_secondByte; // Defined as length of extra data for some events

		unsigned char* m_dataArr; // For most events this is ignored
	};

	// Special MIDI Events that must be written to file
	

	// Monolithic file io functions for reading and writing midi events

	// Note Off               //
	void read0x8_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0x8_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);
	// Note On               //
	void read0x9_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0x9_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);
	// Polyphonic Aftertouch //
	void read0xA_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0xA_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);
	// Control/Mode Change   //
	void read0xB_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0xB_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);
	// Program Change        //
	void read0xC_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0xC_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);
	// Channel Aftertouch    //
	void read0xD_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0xD_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);
	// Pitch Bend Change     //
	void read0xE_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0xE_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);
	// System Messages       //
	void read0xF_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status);
	void write0xF_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status);


	void PackToVariableOutput(std::ofstream&, unsigned long);
	unsigned long UnpackVariableOutput(std::ifstream&);


	MIDIHandler::MIDIHandler()
		: m_rootDirectory("") {

		// Set read and write functions for each StatusHandler
		m_statusHandler[MIDIStatusCodes[x8_]] = { read0x8_, write0x8_, MIDIStatusCodes[x8_] };
		m_statusHandler[MIDIStatusCodes[x9_]] = { read0x9_, write0x9_, MIDIStatusCodes[x9_] };
		m_statusHandler[MIDIStatusCodes[xA_]] = { read0xA_, write0xA_, MIDIStatusCodes[xA_] };
		m_statusHandler[MIDIStatusCodes[xB_]] = { read0xB_, write0xB_, MIDIStatusCodes[xB_] };
		m_statusHandler[MIDIStatusCodes[xC_]] = { read0xC_, write0xC_, MIDIStatusCodes[xC_] };
		m_statusHandler[MIDIStatusCodes[xD_]] = { read0xD_, write0xD_, MIDIStatusCodes[xD_] };
		m_statusHandler[MIDIStatusCodes[xE_]] = { read0xE_, write0xE_, MIDIStatusCodes[xE_] };
		m_statusHandler[MIDIStatusCodes[xF_]] = { read0xF_, write0xF_, MIDIStatusCodes[xF_] };
	}

	MIDIHandler::~MIDIHandler() {

	}

	void MIDIHandler::setRootDirectory(const std::string& rootDir) {

		m_rootDirectory = rootDir;
	}

	void MIDIHandler::writeToMIDI(Phrase* outputPhrase, const std::string& filename) {

		std::ofstream fileOut(m_rootDirectory + filename);
		if (!fileOut.is_open()) {

			std::cout << "Failed to open file " << filename << " for writing." << std::endl;
			return;
		}

		// Setup the header chunk for writing
		MThdChunk mthdChunk = expectedMthd;
		reverseBytes(mthdChunk.size);
		mthdChunk.format    = midi_SingleTrack;
		reverseBytes(mthdChunk.format);
		mthdChunk.numTracks = 0x0001;
		reverseBytes(mthdChunk.numTracks);
		mthdChunk.division = 0x01E0; // Ticks per quarter is equal to 100, ticks per 16th is 25
		//mthdChunk.division = 0x0004; // Ticks per quarter is equal to 4, ticks per 16th is  1
		reverseBytes(mthdChunk.division);

		// Write header chunk to the file
		fileOut.write(reinterpret_cast<char*>(&mthdChunk), sizeof(MThdChunk) - 2);
		
		// Unreverse for later conversions
		reverseBytes(mthdChunk.division);

		// Setup and write track header to the file
		MTrkChunk mtrkChunk = expectedMtrk;
		mtrkChunk.size = 0;
		fileOut.write(reinterpret_cast<char*>(&mtrkChunk), sizeof(MTrkChunk) / 2);
		
		std::streampos trackStart = fileOut.tellp(); // Save stream position to write size later
		fileOut.write(reinterpret_cast<char*>(&mtrkChunk), sizeof(MTrkChunk) / 2);

		// Convert the phrase into a vector of MIDI events
		dumpPhraseToQueue(outputPhrase);

		// Loop over all events and write them to the file
		unsigned int eventNum = static_cast<unsigned int>(m_eventQueue.size());
		unsigned long currentTick = 0;
		unsigned long lastTick = 0;
		
		// TODO: 
		// Figure out what messages need to be sent intially to setup the MIDI file
		// I suspect it needs tempo, program number, and others. maybe use the input files as reference
		// Events: FF 58 04 04 02 24 08 (Time Signature, 4/4 time  )
		//		   FF 51 03 0B 71 B0	(Tempo, 80 bpm			   )
		//		   C0 01				(Instrument, Acoustic Piano)

		MIDIEvent timeSignature;
		timeSignature.m_statusByte = 0xFF;
		timeSignature.m_firstByte  = 0x58;
		timeSignature.m_secondByte = 0x04;
		timeSignature.m_dataArr = new unsigned char[0x04] { 0x04, 0x02, 0x24, 0x08 };

		MIDIEvent tempo;
		tempo.m_statusByte = 0xFF;
		tempo.m_firstByte  = 0x51;
		tempo.m_secondByte = 0x03;
		tempo.m_dataArr    = new unsigned char[0x03]{ 0x0B, 0x71, 0xB0 };

		StatusHandler& specialHandler = m_statusHandler[timeSignature.m_statusByte & 0xF0];
		PackToVariableOutput(fileOut, 0x00);
		specialHandler.writeFunction(fileOut, timeSignature, timeSignature.m_statusByte);
		PackToVariableOutput(fileOut, 0x00);
		specialHandler.writeFunction(fileOut, tempo, tempo.m_statusByte);

		delete[] timeSignature.m_dataArr;
		delete[] tempo.m_dataArr;

		for (unsigned int i = 0; i < eventNum; ++i) {

			// Grab the next event from the queue
			const MIDIEvent& mEvent = m_eventQueue[i];

			// Calculate the delta time of this event and update tick information
			currentTick = arrayIndexToTicksPerQuarter(mEvent.m_subDivisionPosition, mthdChunk.division);
			PackToVariableOutput(fileOut, currentTick - lastTick);
			lastTick = currentTick;

			const StatusHandler& handler = m_statusHandler[mEvent.m_statusByte & 0xF0];
			// Crash here indicates invalid event type
			handler.writeFunction(fileOut, mEvent, mEvent.m_statusByte); 
		}
		
		// Create the end track event
		MIDIEvent endTrackEvent;
		endTrackEvent.m_statusByte = 0xFF;
		endTrackEvent.m_firstByte = 0x2F;
		endTrackEvent.m_secondByte = 0x00;

		// Get the handler and write the event
		specialHandler = m_statusHandler[endTrackEvent.m_statusByte & 0xF0];
		PackToVariableOutput(fileOut, 0x00);
		specialHandler.writeFunction(fileOut, endTrackEvent, endTrackEvent.m_statusByte);

		// Calculate the length of the track chunk in bytes and write it under Mtrk header
		std::streampos trackEnd = fileOut.tellp();
		int trackSize = static_cast<int>(trackEnd - trackStart - 4);
		fileOut.seekp(trackStart);
		reverseBytes(trackSize);
		fileOut.write(reinterpret_cast<char*>(&trackSize), sizeof(int));
	}

	// NOTE: MIDI for some reason is written in big endian not little. 
	//       so when reading/writing we need to reverse the byte ordering

	void MIDIHandler::readFromMIDI(Phrase* inputPhrase, const std::string& filename) {

		// In the process of reading we filter out events we dont care about
		// (pitch wheel changes, control changes, etc.)
		// The queue should only end up holding the note on and note off events
		// that we can write into our phrase after finishing the file

		std::ifstream fileIn(m_rootDirectory + filename);
		if (!fileIn.is_open()) {
			std::cout << "Failed to open file " << filename << " for reading." << std::endl;
			return;
		}

		MThdChunk actualMthd;
		fileIn.read(reinterpret_cast<char*>(&actualMthd), sizeof(actualMthd) - 2);
		reverseBytes(actualMthd.size);
		reverseBytes(actualMthd.format);
		reverseBytes(actualMthd.numTracks);
		reverseBytes(actualMthd.division);

		if (HEADER_NOT_EQUAL(actualMthd.ID, expectedMthd.ID)) {
			std::cout << "Invalid MIDI file, missing MThd chunk ID" << std::endl;
			return;
		}

		if (actualMthd.format != midi_SingleTrack) {
			std::cout << "Invalid MIDI file, Genetic Music only accepts single track MIDI files.\n";
			std::cout << "Expected 1 track, found: " << actualMthd.numTracks <<std::endl;
			//return;
		}

		unsigned int ticksPerQuarter = 0;
		short bitManip = actualMthd.division;
		if (bitManip & 0x8000) { // Check division format (MSB == 1 is fps based)
			// FPS based, bits 14 - 8 are -SMPTE format (frames per second basically) (7 bits)
			//			  bits  7 - 0 are ticks per frame							  (8 bits)
			char SMPTE		  = (bitManip & (0x7F << 8));
			char framesPerSec = (bitManip &  0xFF);
			unsigned int ticksPerSecond = static_cast<int>(SMPTE) * static_cast<int>(framesPerSec);
			ticksPerQuarter = ticksPerSecondToTicksPerQuarter(ticksPerSecond);
		}
		else {
			// easiest to deal with
			ticksPerQuarter = bitManip;
		}

		MTrkChunk actualMtrk;
		fileIn.read(reinterpret_cast<char*>(&actualMtrk), sizeof(MTrkChunk));
		reverseBytes(actualMtrk.size);
		while (HEADER_NOT_EQUAL(actualMtrk.ID, expectedMtrk.ID) && !fileIn.eof()) {

			int ignoreSize = actualMtrk.size;
			fileIn.ignore(ignoreSize);
			fileIn.read(reinterpret_cast<char*>(&actualMtrk), CHUNK_HEADER_SIZE);
		}
		if (fileIn.eof()) {

			std::cout << "Failed to find MTrk chunk in file " << filename << std::endl;
			return;
		}
	
		// At this point file pointer is at the very first timecode for the file
		// we need to go through and extract each event
		unsigned int currentTick = 0;
		MIDIEvent eventStorage;
		do {
			int deltaTime = UnpackVariableOutput(fileIn);
			currentTick += deltaTime;
			eventStorage.m_statusByte = fileIn.get();
			const StatusHandler& handler = m_statusHandler[eventStorage.m_statusByte & 0xF0];
			// If a crash happens here it means the file makes use of MIDI running status
			handler.readFunction(fileIn, eventStorage, eventStorage.m_statusByte);
			
			if ((eventStorage.m_statusByte >> 4) == 0x8 || (eventStorage.m_statusByte >> 4) == 0x9) {

				eventStorage.m_subDivisionPosition = ticksPerQuarterToArrayIndex(currentTick, ticksPerQuarter);
				m_eventQueue.push_back(eventStorage);
			}
		  // Checking for specific byte sequence that means "End of MIDI Track"
		} while (!(eventStorage.m_statusByte == 0xFF && eventStorage.m_firstByte == 0x2F && eventStorage.m_secondByte == 0x00));
		
		// Convert from the matched pairs in the priority queue to elements in the phrase arrays
		// I think that we only need to use the note on events, since when one event starts the previous one ends
		// as the melody is monophonic. Later using harmonic tones we'll need to track note on and off commands

		dumpQueueToPhrase(inputPhrase);

		std::cout << "Finished extracting events from the midi file" << std::endl;
	}

	unsigned int MIDIHandler::ticksPerQuarterToArrayIndex(unsigned int ticks, unsigned int division) {

		return ticks / (division / 4);
	}

	unsigned int MIDIHandler::ticksPerSecondToTicksPerQuarter(unsigned int ticks) {

		const unsigned int beats = 80; // This needs to be gotten from the config settings
		// 60 comes from the conversion from bpm to bps
		return static_cast<unsigned int>((60.0f * static_cast<float>(ticks)) / (static_cast<float>(beats)));
	}

	unsigned int MIDIHandler::arrayIndexToTicksPerQuarter(unsigned int arrayIndex, unsigned int division) {

		return static_cast<unsigned int>((static_cast<float>(division) * (static_cast<float>(arrayIndex) / 4.0f)));
	}

	void MIDIHandler::dumpQueueToPhrase(Phrase* phrase) {
		
		unsigned int numEvents = static_cast<unsigned int>(m_eventQueue.size());
		unsigned int maxNotes = Phrase::_numMeasures * Phrase::_smallestSubdivision;
		for(unsigned int i = 0; i < numEvents; ++i) {
			
			// First grab the current event
			const MIDIEvent& currEvent = m_eventQueue[i];

			// Determine if this is a note off event (otherwise it's a note on event)
			if (currEvent.m_secondByte == 0x00 || IS_NOTE_OFF(currEvent.m_statusByte)) {

				// Skip it since we've already used it with a previous note on event
				continue;
			}

			// Get the array position for this on event (where we write the note)
			unsigned int index = currEvent.m_subDivisionPosition;

			// If this is outside the supported range for a phrase, stop immediately
			if (index > maxNotes) {
				std::cout << "Attempted to load a MIDI file longer than current allowed phrase length" << std::endl;
				std::cout << "Phrase #" << phrase->_phraseID << " may be partially completed" << std::endl;
				break;
			}

			unsigned int noteLen = 0;

			// Loop through the rest of the list to find the corresponding note off event
			for (unsigned int j = i + 1; j < numEvents; ++j) {

				const MIDIEvent& offCandidate = m_eventQueue[j];

				// Check for note off event
				if (offCandidate.m_secondByte == 0x00 || IS_NOTE_OFF(offCandidate.m_statusByte)) {

					// See if this event matches key number with the current on event
					if (currEvent.m_firstByte == offCandidate.m_firstByte) {

						// Find the length of the note
						noteLen = offCandidate.m_subDivisionPosition - index;
						break;
					}
				}
			}

			// Position already has information written
			if (phrase->_melodicData[index]) {

				// We can handle this a couple ways, one would be to just skip this note event
				// thereby just using the first pitch each tick. 
				// We can also just write everything, resulting in just using the last pitch listed
				// Or we can try and split a single track over multiple phrases which would be really
				// messy to try and maintain the original musicality
				
				// Implementing option 1 here:
				continue;

				// Comment out this if statement to implement option 2
			}

			// Write the pitch and rhythmic information to the file
			phrase->_melodicNotes += 1;
			phrase->_melodicData[index]   = currEvent.m_firstByte;
			phrase->_melodicRhythm[index] = noteLen;
		}

		// Do a second pass on the phrase to correct any gaps created by reading rests from the file
		char* rhythm  = phrase->_melodicRhythm;

		// Loop over the entire array
		for (unsigned int note = 0; note < maxNotes;) {

			unsigned int noteVal = rhythm[note];
			// Gap indicates a rest
			if (noteVal == 0) {

				// Find the end of the rest
				unsigned int restLen = 1;
				while ((note + restLen) < maxNotes			  && 
					   restLen < Phrase::_smallestSubdivision && 
					   !(rhythm[note + restLen])) {

					restLen += 1;
				}
				// On loop exit restLen should be the number of subdivisions till the next note begins
				noteVal = restLen;
				rhythm[note] = restLen;
				phrase->_melodicNotes += 1;
			}

			note += noteVal;
		}

		m_eventQueue.clear();
	}

	constexpr unsigned char MezzoForte = 79; // Super arbitrary

	void MIDIHandler::dumpPhraseToQueue(Phrase* phrase) {

		unsigned int maxNotes = Phrase::_numMeasures * Phrase::_smallestSubdivision;
		char* pitches = phrase->_melodicData;
		char* rhythm  = phrase->_melodicRhythm;

		MIDIEvent noteOn;
		noteOn.m_statusByte = MIDIStatusCodes[x9_]; // Note On status byte
		noteOn.m_secondByte = MezzoForte; // Note On velocity

		MIDIEvent noteOff;
		noteOff.m_statusByte = MIDIStatusCodes[x8_]; // Note Off status Byte
		noteOff.m_secondByte = 0; // Note off velocity

		for (unsigned int note = 0; note < maxNotes;) {

			// Read current note pitch
			noteOn.m_firstByte = pitches[note];
			noteOff.m_firstByte = pitches[note];

			// Read note length
			unsigned int noteLength = rhythm[note];

			if (noteOn.m_firstByte != 0) {
				
				noteOn.m_subDivisionPosition  = note;
				noteOff.m_subDivisionPosition = note + noteLength;

				m_eventQueue.push_back(noteOn);
				m_eventQueue.push_back(noteOff);
			}
			
			// Move next note index along
			note += noteLength;
		}

		MIDIComparison comparator;
		std::sort(m_eventQueue.begin(), m_eventQueue.end(), comparator);
	}

	// For handing MIDI status bytes:
	// Status bytes: C_, D_, & F3 have one byte after status
	//				 8_, 9_, A_, B_, E_, F1(?), & F2 have 2 bytes after status
	//               F0 is weird, any number of data bytes, immediately after status byte is a variable length quantity telling number of bytes after the quantity
	//				 F(4-E) is probably 0 bytes after status?
	//			     FF is also really weird, it has a second status byte that determines what else happens
	//				 (we probably just won't code FF and if we find one say "no")

	// For data bytes: theres no real standard for how note velocity maps to volume,
	//				   0 is generally a note off command, but besides that its up to you
	//				   a lot of people recommend a logarithmic scale however


	void PackToVariableOutput(std::ofstream& file, unsigned long value) {

		// Sheer off the bottom 7 bits of our value and insert into temp
		unsigned long temp = value & 0x7F;

		// if the value still has bits set after the first 7 we have more packing to do
		while (value >>= 7) {

			// Shift over the temporary storage so we can write to the least significant 8 bits
			temp <<= 8;

			// Copy the bottom 7 bits from value into temp and set MSB bit
			// MSB set indicates theres more bits involved in the number
			temp |= ((value & 0x7F) | 0x80);
		}

		int i = 0;
		while (true) {

			// Write a character from the temp value into the buffer
			file.put(static_cast<char>(temp));
			if (temp & 0x80) {
				temp >>= 8;
			}
			else {
				break;
			}

		}

	}

	unsigned long UnpackVariableOutput(std::ifstream& file) {

		unsigned long value;
		int i = 0;

		// Extract most significant set of bits from the buffer
		value = static_cast<char>(file.get());

		// Check the MSB from this set to see if we have more bytes to read
		if (value & 0x80) {

			// Remove set MSB and preserve lower 7 (actual data part)
			value &= 0x7F;
			char temp;
			do {
				// Remove the next byte from the buffer
				temp = static_cast<char>(file.get());

				// Shift value and set the bottom 7 bits to extracted values
				value = (value << 7) + (temp & 0x7F);

			} while (temp & 0x80); // Check for additional bits to read
		}

		// Return the value
		return value;
	}
	
	bool MIDIHandler::MIDIComparison::operator()(const MIDIEvent& midiA, const MIDIEvent& midiB) const {
		return (midiA.m_subDivisionPosition < midiB.m_subDivisionPosition);
	}

	// Note Off
	void read0x8_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {

		mEvent.m_firstByte = midiFile.get();
		mEvent.m_secondByte = midiFile.get();
	}

	void write0x8_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {

		midiFile.put(mEvent.m_statusByte);
		midiFile.put(mEvent.m_firstByte);
		midiFile.put(mEvent.m_secondByte);
	}

	// Note On
	void read0x9_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {

		mEvent.m_firstByte = midiFile.get();
		mEvent.m_secondByte = midiFile.get();
	}

	void write0x9_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {

		midiFile.put(mEvent.m_statusByte);
		midiFile.put(mEvent.m_firstByte);
		midiFile.put(mEvent.m_secondByte);
	}

	// Polyphonic Aftertouch
	void read0xA_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {

		mEvent.m_firstByte  = midiFile.get();
		mEvent.m_secondByte = midiFile.get();
	}

	void write0xA_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {


	}

	// Control/Mode Change
	void read0xB_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {

		mEvent.m_firstByte  = midiFile.get();
		mEvent.m_secondByte = midiFile.get();
	}

	void write0xB_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {

	}

	// Program Change
	void read0xC_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {
		
		mEvent.m_firstByte = midiFile.get();
	}

	void write0xC_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {


	}

	// Channel Aftertouch
	void read0xD_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {

		mEvent.m_firstByte = midiFile.get();
	}

	void write0xD_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {

	}

	// Pitch Bend Change
	void read0xE_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {

		mEvent.m_firstByte  = midiFile.get();
		mEvent.m_secondByte = midiFile.get();
	}

	void write0xE_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {

	}

	// System Messages
	void read0xF_(std::ifstream& midiFile, MIDIEvent& mEvent, const unsigned char& status) {

		unsigned long messageLength;

		switch (status) {

		case 0xF0: // System Exclusive (SYSEX)
			messageLength = UnpackVariableOutput(midiFile);
			midiFile.ignore(messageLength);
			mEvent.m_firstByte  = 0;
			mEvent.m_secondByte = 0;
			break;

		case 0xF1: // MIDI Time Code Qtr. Frame
			mEvent.m_firstByte  = midiFile.get();
			mEvent.m_secondByte = midiFile.get();
			break;

		case 0xF2: // Song Position Pointer
			mEvent.m_firstByte  = midiFile.get();
			mEvent.m_secondByte = midiFile.get();
			break;

		case 0xF3: // Song Select
			mEvent.m_firstByte  = midiFile.get();
			mEvent.m_secondByte = 0x00;
			break;

		case 0xF4: // Undefined
			std::cout << "Undefined event found " << status << std::endl;
			break;

		case 0xF5: // Undefined
			std::cout << "Undefined event found " << status << std::endl;
			break;

		case 0xF6: // Tune Request
			break;

		case 0xF7: // End of SysEx (EOX)
			break;

		case 0xF8: // Timing Clock
			break;

		case 0xF9: // Undefined
			break;

		case 0xFA: // Start
			break;

		case 0xFB: // Continue
			break;

		case 0xFC: // Stop
			break;

		case 0xFD: // Undefined
			std::cout << "Undefined event found " << status << std::endl;
			break;

		case 0xFE: // Active Sensing
			break;

		case 0xFF: // Special event codes
			mEvent.m_firstByte = midiFile.get();
			switch (mEvent.m_firstByte) {

			case 0x00: // Sequence Number
				if (midiFile.get() == 0x20) {
					mEvent.m_firstByte = midiFile.get();
					mEvent.m_secondByte = midiFile.get();
				}
				break;

			case 0x01: // Text
			case 0x02: // Copyright
			case 0x03: // Sequence/Track Name
			case 0x04: // Instrument
			case 0x05: // Lyric
			case 0x06: // Marker
			case 0x07: // Cue Point
				messageLength = UnpackVariableOutput(midiFile);
				midiFile.ignore(messageLength);
				mEvent.m_firstByte  = 0;
				mEvent.m_secondByte = 0;
				break;

			case 0x20: // MIDI Channel
			case 0x21: // MIDI Port
				mEvent.m_firstByte  = midiFile.get();
				mEvent.m_secondByte = midiFile.get();
				break;

			case 0x2F: // End of Track message
				mEvent.m_secondByte = midiFile.get();
				break;

			case 0x51: // Tempo mark, after second byte three byte that combine to form tempo measure
				mEvent.m_secondByte = midiFile.get();
				midiFile.ignore(3); // 3 bytes to combine into tempo measure
				break;

			case 0x54: // SMPTE Offset
				mEvent.m_secondByte = midiFile.get();
				midiFile.ignore(5);  // 5 bytes to store start time offset of the track
				break;

			case 0x58: // Time Signature
				mEvent.m_secondByte = midiFile.get();
				midiFile.ignore(4); // 4 bytes to store time signature information
				break;

			case 0x59: // Key Signature
				mEvent.m_secondByte = midiFile.get();
				midiFile.ignore(2); // 2 bytes to store sharps/flats and major/minor
				break;

			case 0x7F: // Proprietary Event
				messageLength = UnpackVariableOutput(midiFile);
				midiFile.ignore(messageLength);
				mEvent.m_firstByte  = 0;
				mEvent.m_secondByte = 0;
				break;
			}
			break;
		}
	}

	void write0xF_(std::ofstream& midiFile, const MIDIEvent& mEvent, const unsigned char& status) {

		//unsigned long messageLength;
		midiFile.put(mEvent.m_statusByte);

		switch (status) {

		case 0xF0: // System Exclusive (SYSEX)
			break;

		case 0xF1: // MIDI Time Code Qtr. Frame
			midiFile.put(mEvent.m_firstByte);
			midiFile.put(mEvent.m_secondByte);
			break;

		case 0xF2: // Song Position Pointer
			midiFile.put(mEvent.m_firstByte);
			midiFile.put(mEvent.m_secondByte);
			break;

		case 0xF3: // Song Select
			break;

		case 0xF4: // Undefined
			std::cout << "Undefined event found " << status << std::endl;
			break;

		case 0xF5: // Undefined
			std::cout << "Undefined event found " << status << std::endl;
			break;

		case 0xF6: // Tune Request
			break;

		case 0xF7: // End of SysEx (EOX)
			break;

		case 0xF8: // Timing Clock
			break;

		case 0xF9: // Undefined
			break;

		case 0xFA: // Start
			break;

		case 0xFB: // Continue
			break;

		case 0xFC: // Stop
			break;

		case 0xFD: // Undefined
			std::cout << "Undefined event found " << status << std::endl;
			break;

		case 0xFE: // Active Sensing
			break;

		case 0xFF: // Special event codes

			midiFile.put(mEvent.m_firstByte);
			switch (mEvent.m_firstByte) {

			case 0x00: // Sequence Number

				break;

			case 0x01: // Text
			case 0x02: // Copyright
			case 0x03: // Sequence/Track Name
			case 0x04: // Instrument
			case 0x05: // Lyric
			case 0x06: // Marker
			case 0x07: // Cue Point

				break;

			case 0x20: // MIDI Channel
			case 0x21: // MIDI Port

				break;

			case 0x2F: // End of Track message
				midiFile.put(mEvent.m_secondByte);
				break;

			case 0x51: // Tempo mark, after second byte three byte that combine to form tempo measure
				midiFile.put(mEvent.m_secondByte);
				for (int i = 0; i < mEvent.m_secondByte; ++i) {
					midiFile.put(mEvent.m_dataArr[i]);
				}
				break;

			case 0x54: // SMPTE Offset

				break;

			case 0x58: // Time Signature
				midiFile.put(mEvent.m_secondByte);
				for (int i = 0; i < mEvent.m_secondByte; ++i) {
					midiFile.put(mEvent.m_dataArr[i]);
				}
				break;

			case 0x59: // Key Signature

				break;

			case 0x7F: // Proprietary Event

				break;
			}
			break;
		}

	}


} // namespace Genetics