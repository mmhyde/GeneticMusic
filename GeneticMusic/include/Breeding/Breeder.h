#pragma once

#include <random>
#include <utility>

#include "PhrasePool.h"

namespace Genetics {

	struct Phrase;
	typedef std::pair<Phrase*, Phrase*> BreedingPair;

	class Breeder
	{
	public:
		Breeder();
		~Breeder();

		void Breed(const BreedingPair& parents, unsigned measureCount, unsigned subdivision);

	private:
		std::mt19937 m_randomEngine;

		char* m_tempBuffer;
		unsigned m_tempBufferSize;

	};

	struct CrosspointBreed {

		CrosspointBreed() 
			: m_tempBuffer(new char[256]), m_tempBufferSize(256) { 
			
		}

		~CrosspointBreed() { 

			if (m_tempBuffer != nullptr) {

				delete[] m_tempBuffer;
			}
		}

	protected:
		void CreateChildren(const BreedingPair& parents, PhrasePool* phrasePool);

	private:

		char* m_tempBuffer;
		unsigned m_tempBufferSize;
	};

	struct InterpolateBreed {

	protected:
		InterpolateBreed() {

			std::random_device rd;
			m_randomEngine.seed(rd());
		}

		void CreateChildren(const BreedingPair& parents, PhrasePool* phrasePool);
	
	private:
		std::mt19937 m_randomEngine;

	};

	template <class Policy>
	class BreedingMethod : public Policy {

	public:
		BreedingMethod();
		virtual ~BreedingMethod();

		void Breed(const BreedingPair& parents, PhrasePool* phrasePool);

	private:
		std::mt19937 m_randomEngine;
		char* m_tempBuffer;


	};

	template <class Policy>
	BreedingMethod<Policy>::BreedingMethod() {

	}

	template <class Policy>
	BreedingMethod<Policy>::~BreedingMethod() {

	}
	
	template <class Policy>
	void BreedingMethod<Policy>::Breed(const BreedingPair& parents, PhrasePool* phrasePool) {

		Policy::CreateChildren(parents, phrasePool);
	}


} // namespace Genetics