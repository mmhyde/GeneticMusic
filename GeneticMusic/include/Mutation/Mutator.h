#pragma once

#include <random>

namespace Genetics {

	struct Phrase;
	class Mutator;

	typedef void (Mutator::*Mutation)(Phrase*);

	/*
	struct MutationBase {

		virtual int16_t getWeight() const = 0;
		virtual void operator()(Phrase* phrase) const = 0;
	};

	struct NullOperator : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct Subdivide : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct Merge : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct Rest : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct Rotate : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct Transpose : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct SortAscending : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct SortDescending : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct Inversion : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};

	struct Retrograde : public MutationBase {

		int16_t getWeight() const override;
		void operator()(Phrase* phrase) const override;
	};
	*/

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
		//std::vector<MutationBase*> m_mutationPool2;

		unsigned m_numMutations;
	};

} // namespace Genetics