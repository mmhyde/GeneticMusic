// Morgen Hyde

#pragma once

namespace Genetics {

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

	const unsigned int CHUNK_HEADER_SIZE = 8;

	const float MAX_16 = float((1 << 15) - 1);
	const float MAX_32 = float((1 << 31) - 1);

	const RIFFChunk expectedRiff = { {'R','I','F','F'}, 0, {'W','A','V','E'} };
	const fmt_Chunk expectedFmt  = { {'f','m','t',' '} };
	const dataChunk expectedData = { {'d','a','t','a'} };

} // namespace Genetics