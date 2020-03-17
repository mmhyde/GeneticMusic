
#include "TemplateGarbage/LokiAllocators.h"

#include <cassert>

FixedAllocator::FixedAllocator() {

	
}

FixedAllocator::~FixedAllocator() {


}

void* FixedAllocator::Allocate() {

	// Check if the last chunk we used has memory available
	if (_allocChunk == 0 || _allocChunk->_blocksAvailable == 0) {
		// It didn't so we gotta find a new chunk
		Chunks::iterator iter = _chunks.begin();
		for (;; ++iter) {

			if (iter == _chunks.end()) {

				// _chunks is full so we need to allocate a new chunk
				_chunks.reserve(_chunks.size() + 1);
				Chunk newChunk;
				newChunk.Init(_blockSize, _numBlocks);
				_chunks.push_back(newChunk);
				_allocChunk = &_chunks.back();
				_deallocChunk = &_chunks.back();
				break;
			}

			if (iter->_blocksAvailable > 0) {
				// We found a chunk with space for the allocation
				_allocChunk = &(*iter);
				break;
			}
		}
	}

	assert(_allocChunk != 0);
	assert(_allocChunk->_blocksAvailable > 0);
	return _allocChunk->Allocate(_blockSize);
}

inline bool PtrInChunk(Chunk* chunk, size_t blockWidth, void* ptr) {

	return (chunk->_pData <= ptr) && (chunk->_pData + blockWidth >= ptr);
}

void FixedAllocator::Deallocate(void* ptr) {

	if (_deallocChunk == 0) {
		
		_deallocChunk = &_chunks.front();
		_deallocPosition = 0;
	}

	if (!PtrInChunk(_deallocChunk, _blockSize, ptr)) {

		Chunks::iterator reverseIter, forwardIter;
		reverseIter = (_chunks.begin() + _deallocPosition);
		forwardIter = (_chunks.begin() + _deallocPosition);

		while (reverseIter != _chunks.begin() && forwardIter != _chunks.end()) {

			if (reverseIter != _chunks.begin()) {

				--reverseIter;
				if (PtrInChunk(&*reverseIter, _blockSize, ptr)) {

					_deallocChunk = &*reverseIter;
					break;
				}
			}

			if (forwardIter != _chunks.end()) {

				++forwardIter;
				if (PtrInChunk(&*forwardIter, _blockSize, ptr)) {

					_deallocChunk = &*forwardIter;
					break;
				}
			}
		}
	}

	assert(_deallocChunk != 0);
	assert(_deallocChunk->_blocksAvailable < _numBlocks);
	_deallocChunk->Deallocate(ptr, _blockSize);
}

void Chunk::Init(size_t blockSize, unsigned char numBlocks) {

	// Allocate initial memory
	_pData = new unsigned char[blockSize * numBlocks];

	// Setup info variables
	_firstAvailableBlock = 0;
	_blocksAvailable = numBlocks;

	// Initialize the blocks to contain the index of the next available block
	unsigned char i = 0;
	unsigned char* ptr = _pData;
	for (; i != numBlocks; ptr += blockSize) {
		*ptr = ++i;
	}
}

void* Chunk::Allocate(size_t blockSize) {

	// Check if we have blocks to allocate
	if (!_blocksAvailable) {
		return nullptr;
	}

	// Get a pointer to the next available block
	unsigned char* pResult = _pData + (_firstAvailableBlock * blockSize);

	// Get the index of the next next block
	_firstAvailableBlock = *pResult;
	--_blocksAvailable;

	// Return a pointer to the block
	return pResult;
}

void Chunk::Deallocate(void* ptr, size_t blockSize) {

	// Check the pointer is at least after pData in memory
	assert(ptr >= _pData);
	unsigned char* toRelease = static_cast<unsigned char*>(ptr);

	// Checking for alignment
	assert((toRelease - _pData) % blockSize == 0);
	*toRelease = _firstAvailableBlock;
	_firstAvailableBlock = static_cast<unsigned char>((toRelease - _pData) / blockSize);

	// Truncation Check
	assert(_firstAvailableBlock == (toRelease - _pData) / blockSize);
	++_blocksAvailable;
}