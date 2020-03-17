// Morgen Hyde

#pragma once

#include <random>
#include <queue>
#include <cstdint>

namespace Genetics {

	typedef uint64_t ID;

	template <class Type>
	class IDGenerator {

	public:

		static IDGenerator& getInstance() {
			static IDGenerator gen;
			return gen;
		}

		~IDGenerator();

		IDGenerator(const IDGenerator&) = delete;
		IDGenerator& operator=(const IDGenerator&) = delete;

		static ID getID();

	private:

		IDGenerator();
	};



}