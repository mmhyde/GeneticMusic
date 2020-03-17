#pragma once


#include <vector>

struct Chunk;

class FixedAllocator {

public:

	FixedAllocator();
	~FixedAllocator();

	void* Allocate();
	void Deallocate(void* ptr);

private:

	size_t _blockSize;
	unsigned char _numBlocks;
	unsigned char _deallocPosition;
	typedef std::vector<Chunk> Chunks;
	Chunks _chunks;
	Chunk* _allocChunk;
	Chunk* _deallocChunk;
};

struct Chunk {
	void Init(size_t blockSize, unsigned char numBlocks);
	void* Allocate(size_t blockSize);
	void Deallocate(void* ptr, size_t blockSize);

	unsigned char* _pData;
	unsigned char _firstAvailableBlock, _blocksAvailable;
};

