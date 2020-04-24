#pragma once

#include <exception>
#include <cstring>
#include <cstdint>

namespace Genetics {

	template <typename T>
	class PoolAllocator {

	public:
		PoolAllocator(uint32_t maxObjects);
		~PoolAllocator();

		T* alloc();
		void free(T* object);

		void clear();

		__inline uint32_t capacity() const {
			return m_maxObjects;
		}

	private:
		struct ObjNode {
			ObjNode() 
				: m_next(nullptr) { }

			ObjNode* m_next;
		};

		void buildPool();

		ObjNode* m_freeList;

		char* m_objectArray;
		uint32_t m_maxObjects;
		uint32_t m_numObjects;
	};

	template <typename T>
	PoolAllocator<T>::PoolAllocator(uint32_t maxObjects)
		: m_maxObjects(maxObjects), m_numObjects(0) {

		// Allocate all the memory we need for the pool upfront and 0 it out
		m_objectArray = new char[maxObjects * sizeof(T)];
		std::memset(m_objectArray, 0, sizeof(T) * m_maxObjects);		

		buildPool();
	}

	template <typename T>
	PoolAllocator<T>::~PoolAllocator() {

		// Zero out the memory
		std::memset(m_objectArray, 0, sizeof(T) * m_maxObjects);

		// Set list head to null
		m_freeList = nullptr;

		// Clear out the object array
		delete[] m_objectArray;
	
		// Leave it as nullptr
		m_objectArray = nullptr;
	}

	template <typename T>
	T* PoolAllocator<T>::alloc() {

		T* newObject = nullptr;

		// Check if we have any space for allocations
		if (m_freeList) 
		{
			// Grab this object off the top of the allocated list
			ObjNode* allocated = m_freeList;

			// Move the head of the free list to the next object
			m_freeList = m_freeList->m_next;

			// Use placement new to construct the object in the character array
			newObject = new (allocated) T();

			++m_numObjects;
		}

		// Return the object, nullptr if the pool is all used up
		return newObject;
	}

	template <typename T>
	void PoolAllocator<T>::free(T* object) {

		// Check that the object exists
		if (object) {

			// Call the objects destructor and 0 out that memory for safety
			object->~T();
			std::memset(object, 0, sizeof(T));

			char* poolBound = m_objectArray + (sizeof(T) * m_maxObjects);
			if (reinterpret_cast<char*>(object) >= m_objectArray && reinterpret_cast<char*>(object) <= poolBound) {

				ObjNode* node = reinterpret_cast<ObjNode*>(object);
				
				// Make this node the new head of the free list
				node->m_next = m_freeList;
				m_freeList = node;

				--m_numObjects;
			}
		}
	}

	template <typename T>
	void PoolAllocator<T>::clear() {

		std::memset(m_objectArray, 0, sizeof(T) * m_maxObjects);
		buildPool();
		m_numObjects = 0;
	}

	template <typename T>
	void PoolAllocator<T>::buildPool() {

		// Loop over the memory and add it to the free list
		for (unsigned i = 0; i < m_maxObjects; ++i) {

			ObjNode* node = reinterpret_cast<ObjNode*>(m_objectArray + (i * sizeof(T)));
			node->m_next = m_freeList;
			m_freeList = node;
		}
	}

} // namespace Genetics