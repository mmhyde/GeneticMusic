#pragma once

#include <vector>

#include "Phrase.h"
#include "PoolAllocator.h"

namespace Genetics {

	/*
	class PhrasePool
	{
	public:
		PhrasePool(PoolAllocator<Phrase>& poolAlloc, unsigned measureCount, unsigned subDivision);
		~PhrasePool();

		void AddPhrase(Phrase* newPhrase);
		void RemovePhrase(Phrase* oldPhrase);

		Phrase* AllocateEmptyPhrase();
		const std::vector<Phrase*>& GetPhrases() const;

		void ClearPhrases();

		__inline unsigned GetMeasuresPerPhrase() const { return m_measureCount; }
		__inline unsigned GetSmallestSubDivision() const { return m_subDivision; }
		__inline unsigned GetMaxNotes() const { return m_measureCount * m_subDivision; }

		unsigned GetPhraseNumberOf(Phrase* phrase) const;

		void DisplayRatings() const;

	private:
		std::vector<Phrase*> m_population;
		PoolAllocator<Phrase>& m_poolAllocator;

		const unsigned m_measureCount; // number of measures per phrase
		const unsigned m_subDivision;  // smallest subdivision in the phrase
	};
	*/

	using PhraseVec = std::vector<Phrase*>;

	struct ElitistPrune {

		static void Merge(PoolAllocator<Phrase>& poolAlloc, PhraseVec& parents, PhraseVec& children);
	};

	struct GenerationalPrune {
		
		static void Merge(PoolAllocator<Phrase>& poolAlloc, PhraseVec& parents, PhraseVec& children);
	};

	struct TruncationPrune {
		
		static void Merge(PoolAllocator<Phrase>& poolAlloc, PhraseVec& parents, PhraseVec& children);
	};

	
	class PhrasePool{

	public:
		PhrasePool(PoolAllocator<Phrase>& poolAlloc, unsigned measureCount, unsigned subDivision);
		~PhrasePool();

		Phrase* AllocateChild();

		__inline unsigned GetNumParents() const { return static_cast<unsigned>(m_population.size()); }
		const std::vector<Phrase*>& GetPhrases() const { return m_population; }

		__inline unsigned GetNumChildren() const { return static_cast<unsigned>(m_childPopulation.size()); }
		const std::vector<Phrase*>& GetChildren() const { return m_childPopulation; }

		__inline unsigned GetMeasuresPerPhrase() const { return m_measureCount; }
		__inline unsigned GetSmallestSubDivision() const { return m_subDivision; }
		__inline unsigned GetMaxNotes() const { return m_measureCount * m_subDivision; }


		template <class PruningPolicy>
		void MergeChildrenToPopulation();

		unsigned GetPhraseNumberOf(Phrase* phrase) const;
		void DisplayRatings() const;

	private:
		std::vector<Phrase*> m_population;
		std::vector<Phrase*> m_childPopulation;
		PoolAllocator<Phrase>& m_poolAllocator;

		const unsigned m_measureCount;
		const unsigned m_subDivision;
	};

	template <class PruningPolicy>
	void PhrasePool::MergeChildrenToPopulation() {
		PruningPolicy::Merge(m_poolAllocator, m_population, m_childPopulation);
	}

} // namespace Genetics