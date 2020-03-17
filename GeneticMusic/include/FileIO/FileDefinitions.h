// Morgen Hyde
#pragma once

namespace Genetics {

	// RIFF / WAV FILE CHUNK INFORMATION //

	struct RIFFChunk /* size - byte offset - totalOffset */
	{
		/* TOTAL SIZE: 12 bytes*/
		char ID[4];     /* 4 - 0 - 0 */
		int size;       /* 4 - 4 - 4 */
		char type[4]; /* 4 - 8 - 8 */
	};

	struct fmt_Chunk /* size - byte offset - totalOffset */
	{
		/* TOTAL SIZE: 24 bytes*/
		char ID[4];          /* 4 - 0  - 12 */
		int size;			 /* 4 - 4  - 16 */
		short format;		 /* 2 - 8  - 20 */
		short channels;		 /* 2 - 10 - 22 */
		int sampleRate;		 /* 4 - 12 - 24 */
		int byteRate;		 /* 4 - 16 - 28 */
		short blockAlign;    /* 2 - 20 - 32 */
		short bitsPerSample; /* 2 - 22 - 34 */
	};

	struct dataChunk /* size - byte offset */
	{
		/* TOTAL SIZE: 8 bytes*/
		char ID[4]; /* 4 - 0 - 36 */
		int size;   /* 4 - 4 - 40 */
	};

	const RIFFChunk expectedRiff = { {'R','I','F','F'}, 0, {'W','A','V','E'} };
	const fmt_Chunk expectedFmt  = { {'f','m','t',' '} };
	const dataChunk expectedData = { {'d','a','t','a'} };

	// MIDI FILE / CHUNK INFORMATION //
	struct MThdChunk {   /* size - byte offset - totalOffset */
		char ID[4];		 /* 4 -  0 -  0 */
		int  size;		 /* 4 -  4 -  4 */
		short format;	 /* 2 -  8 -  8 */
		short numTracks; /* 2 - 10 - 10 */
		short division;  /* 2 - 12 - 12 */
	};

	// Partial Implementation - MTrk chunks are different than other chunks
	struct MTrkChunk { /* size - byte offset - totalOffset */
		char ID[4]; /* 4 - 14 - 14 */
		int size;   /* 4 - 18 - 18 */
	};

	enum MIDIFormatIDs {
		midi_SingleTrack = 0,
		midi_MultiTrackSimul,
		midi_MultiTrackSequence
	};

	const unsigned char MIDIStatusCodes[] = { 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 };
	enum MIDIStatusCodesEnum {
		x8_ = 0,
		x9_,
		xA_,
		xB_,
		xC_,
		xD_,
		xE_,
		xF_
	};

#define IS_NOTE_OFF(byte) (byte & 0xF0) == 0x80
#define IS_NOTE_ON(byte)  (byte & 0xF0) == 0x90

	const MThdChunk expectedMthd = { {'M', 'T', 'h', 'd'}, 6 };
	const MTrkChunk expectedMtrk = { {'M', 'T', 'r', 'k'} };


	__inline void reverseBytes(short& data) {
		short buffer = data & 0xFF;
		buffer <<= 8;
		buffer |= (data >> 8) & 0xFF;
		data = buffer;
	}

	__inline void reverseBytes(int& data) {
		int buffer = data & 0xFF;
		buffer <<= 8;
		buffer |= (data >> 8) & 0xFF;
		buffer <<= 8;
		buffer |= (data >> 16) & 0xFF;
		buffer <<= 8;
		buffer |= (data >> 24) & 0xFF;
		data = buffer;
	}



	// GENERAL FILE DEFINES //
	const unsigned int CHUNK_HEADER_SIZE = 8;

#pragma warning(push)
#pragma warning(disable : 4307)

	const float MAX_16 = float((1 << 15) - 1);
	const float MAX_32 = float((1 << 31) - 1);

#pragma warning(pop)

#define HEADER_NOT_EQUAL(ID1, ID2) std::memcmp(ID1, ID2, 4) != 0


} // namespace Genetics
