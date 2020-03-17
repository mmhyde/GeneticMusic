#pragma once

#include <random>

namespace Genetics {

	struct Phrase;
	class Mutator;

	typedef void (Mutator::*Mutation)(Phrase*);

	class Mutator
	{
	public:
		Mutator();
		~Mutator();

		void InitMutationPool();

		void Mutate(Phrase* phrase);

	private:
		void NullOperator(Phrase*);

		// Rhythmically based operations
		void Subdivide(Phrase*);
		void Merge(Phrase*);
		void Rest(Phrase*);

		// Pitch based operations
		void Rotate(Phrase*);
		void Transpose(Phrase*);
		void SortAscending(Phrase*);
		void SortDescending(Phrase*);
		void Inversion(Phrase*);
		void Retrograde(Phrase*);

		std::mt19937 m_randomEngine;

		std::vector<short> m_mutationWeights;
		std::vector<Mutation> m_mutationPool;

		unsigned m_numMutations;
	};

} // namespace Genetics