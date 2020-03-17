#pragma once

#include <iostream>

struct Dummy {
	char dArray[16];
};

struct Base {

public:
	Base(float val) {
		std::cout << "Base Constructed" << std::endl;
	}
	~Base() {
		std::cout << "Base Destructed" << std::endl;
	}

protected:
	Dummy* dPtr;
	char bArray[16];
	float bVal;
};

struct X : public Base {

public:
	X(float val)
		: Base(val) {
		std::cout << "X Constructed" << std::endl;
	}
	~X() {
		std::cout << "X Destructed" << std::endl;
	}
};

struct Y : public Base {

public:
	Y(float val)
		: Base(val) {
		std::cout << "Y Constructed" << std::endl;
	}
	~Y() {
		std::cout << "Y Destructed" << std::endl;
	}
};

struct Z : public Base {

public:
	Z(float val)
		: Base(val) {
		std::cout << "Z Constructed" << std::endl;
	}
	~Z() {
		std::cout << "Z Destructed" << std::endl;
	}
};

template <typename... Args>
struct EqualSize {
};

template <typename T1>
struct EqualSize<T1> {

	enum {
		_size = sizeof(T1),
		_equal = 1,
	};
};

template <typename T1, typename... Args>
struct EqualSize<T1, Args...> {

	enum {
		_size = sizeof(T1),
		_equal = (EqualSize<Args...>::_equal && _size == EqualSize<Args...>::_size),
	};
};

template <typename... Args>
struct Variadic {

	Variadic() {
		
		static_assert(EqualSize<Args...>::_equal, "Template parameters of Variadic are not equal size");
	
	
	}

	~Variadic() {


	}


	template <typename... Args>
	struct local { };

	template <typename T1, typename... Args>
	struct local<T1, Args...> {
		
		local() { };
		~local() { }
		
		union {
			T1 value;
			local<Args...> layer;
		};
	};

	template <typename T1>
	struct local<T1> {

		local() { };
		~local() { }

		T1 value;
	};

	local<Args...> m_memory;

};
