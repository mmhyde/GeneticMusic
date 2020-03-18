#pragma once

#include <exception>
#include <cstring>

namespace Genetics {

	template <typename T>
	class PoolAllocator {

	public:
		PoolAllocator(unsigned maxObjects);
		~PoolAllocator();

		T* Alloc();
		void Free(T* object);

		void SetNewPoolSize(unsigned maxObjects);
		void Clear();

		__inline unsigned Capacity() const {
			return m_maxObjects;
		}

	private:
		struct ObjNode {
			ObjNode() 
				: m_memoryAddr(nullptr), m_next(nullptr) {

			}

			char* m_memoryAddr;
			ObjNode* m_next;
		};

		ObjNode* m_freeList;
		ObjNode* m_allocList;

		char* m_objectArray;
		unsigned m_maxObjects;
	};

	template <typename T>
	PoolAllocator<T>::PoolAllocator(unsigned maxObjects)
		: m_maxObjects(maxObjects), m_allocList(nullptr) {

		// Allocate all the memory we need for the pool upfront and 0 it out
		m_objectArray = new char[maxObjects * sizeof(T)];
		std::memset(m_objectArray, 0, sizeof(T) * m_maxObjects);

		// Allocate all the object nodes that we need to store memory addresses
		m_freeList = new ObjNode;
		m_freeList->m_memoryAddr = m_objectArray;

		// Loop over the list of all objects, allocate nodes for them,
		// and add them into the free list
		for (unsigned i = 1; i < maxObjects; ++i) 
		{
			ObjNode* newNode = new ObjNode;
			newNode->m_memoryAddr = m_objectArray + (i * sizeof(T));

			newNode->m_next = m_freeList;
			m_freeList = newNode;
		}
	}

	template <typename T>
	PoolAllocator<T>::~PoolAllocator() {

		std::memset(m_objectArray, 0, sizeof(T) * m_maxObjects);

		// While there are allocated elements in the free list
		ObjNode* listIter = m_freeList;
		while (listIter)
		{
			// Grab the current head
			ObjNode* temp = listIter;
			// Move the head to the next element
			listIter = listIter->m_next;
			delete temp;
		}

		// Next we want to call the destructors on any allocated elements
		// in our raw character array
		listIter = m_allocList;
		while (listIter)
		{
			// Grab the current head of the alloc list
			ObjNode* temp = listIter;

			// Move the head to the next element
			listIter = listIter->m_next;

			// Call the destructor on the memory
			T* allocatedObject = reinterpret_cast<T*>(temp->m_memoryAddr);
			allocatedObject->~T();

			delete temp;
		}

		// Clear out the object array
		delete[] m_objectArray;
	}

	template <typename T>
	T* PoolAllocator<T>::Alloc() {

		T* newObject = nullptr;

		// Check if we have any space for allocations
		if (m_freeList) 
		{
			// Grab this object off the top of the allocated list
			ObjNode* allocated = m_freeList;

			// Move the head of the free list to the next object
			m_freeList = m_freeList->m_next;

			// Extract the index for this object in our character array
			char* objectIndex = allocated->m_memoryAddr;

			// Use placement new to construct the object in the character array
			newObject = new (objectIndex) T();

			// Push the newly allocated object onto the top of the alloc list
			allocated->m_next = m_allocList;
			m_allocList = allocated;
		}

		// Return the object, nullptr if the pool is all used up
		return newObject;
	}

	template <typename T>
	void PoolAllocator<T>::Free(T* object) {

		// Check that the object exists
		if (object)
		{
			// Call the objects destructor and 0 out that memory for safety
			object->~T();
			char* objectAddr = reinterpret_cast<char*>(object);
			std::memset(objectAddr, 0, sizeof(T));

			ObjNode* lastNode = nullptr;
			ObjNode* memoryNode = nullptr;

			// Find the memory address that we just free'd in the allocated list
			ObjNode* listIter = m_allocList;
			while (listIter)
			{
				// If the memory addresses match
				if (listIter->m_memoryAddr == objectAddr) 
				{
					memoryNode = listIter;

					// If there was a previous node, patch the hole we make by
					// removing the current node
					if (lastNode) {
						lastNode->m_next = memoryNode->m_next;
					}
					// If there wasn't we removed the head so set head to nullptr;
					else {
						m_allocList = nullptr;
					}

					// Exit the loop since we found our node
					break;
				}

				// Move to the next element in the list
				lastNode = listIter;
				listIter = listIter->m_next;
			}

			// If for some awful reason we didn't find that address throw an exception?
			// Because something is very wrong
			if (!memoryNode) {
				throw std::exception("Odds are you double free'd something. Nice work.");
			}

			// Add this node to the front of the freeList
			memoryNode->m_next = m_freeList;
			m_freeList = memoryNode;
		}
	}

	template <typename T>
	void PoolAllocator<T>::SetNewPoolSize(unsigned newPoolSize) {

		Clear();
	}

	template <typename T>
	void PoolAllocator<T>::Clear() {

	}

} // namespace Genetics