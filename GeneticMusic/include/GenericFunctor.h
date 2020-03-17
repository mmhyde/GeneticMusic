#pragma once

#include <iostream>

namespace Genetics {

	// Forward declarations
	template <typename ResultType, typename... Args>
	class FunctorImpl;

	template <class FunctorType, typename BoundType, typename... Args>
	class BinderFirst;

	// Main functor class definition, inspired by Loki template library but adapted to use C++11 
	template <typename ResultType, typename... Args>
	class Functor {

	private:
		typedef FunctorImpl<ResultType, Args...> Impl;

	public:
		Functor();
		Functor(const Functor& source);
		Functor& operator=(const Functor& rhs);
		explicit Functor(std::unique_ptr<Impl> spImpl);

		// Specific constructors for callable entities //
		
		// Constructor for functors
		template <typename Funct>
		Functor(const Funct& funct);

		// Constructor for member function pointers
		template <typename ObjectPtr, typename PtrToMemFn>
		Functor(ObjectPtr obj, PtrToMemFn fn);

		// Member functions and Operator overloads //

		// Operator() overload to act as callable entity
		ResultType operator()(Args... args) {
			return (*_spImpl)(args...);
		}

		typedef ResultType Result;

	private:
		std::unique_ptr<Impl> _spImpl;
	};

	// FunctorImpl Declaration //
	template <typename ResultType, typename... Args>
	class FunctorImpl {

	public:
		virtual ResultType operator()(Args... args) = 0;
		virtual FunctorImpl* clone() const = 0;
	};

	// Handler Declarations / Definitions //

	// Functor handler deals with traditional C++ functors as well as standard function pointers
	template <class ParentFunctor, typename Funct, typename... Args>
	class FunctorHandler : public FunctorImpl<typename ParentFunctor::Result, Args...> {

		typedef typename ParentFunctor::Result ReturnType;

	public:
		FunctorHandler(const Funct& funct)
			: _functor(funct) {

		}

		ReturnType operator()(Args... args) override {

			return _functor(std::forward<Args>(args)...);
		}

		FunctorImpl<typename ParentFunctor::Result, Args...>* clone() const override {

			return new FunctorHandler<ParentFunctor, Funct, Args...>(_functor);
		}

	private:
		Funct _functor;
	};

	// Handles member function pointers
	template <class ParentFunctor, typename ObjectPtr, typename PtrToMemFn, typename... Args>
	class MemberFunctionHandler : public FunctorImpl<typename ParentFunctor::Result, Args...> {

		typedef typename ParentFunctor::Result ReturnType;

	public:
		MemberFunctionHandler(ObjectPtr obj, PtrToMemFn function)
			: _object(obj), _memberFunction(function) {

		}

		ReturnType operator()(Args... args) override {
			return (_object->*_memberFunction)(std::forward<Args>(args)...);
		}

		FunctorImpl<typename ParentFunctor::Result, Args...>* clone() const override {

			return new MemberFunctionHandler<ParentFunctor, ObjectPtr, PtrToMemFn, Args...>(_object, _memberFunction);
		}

	private:
		ObjectPtr _object;
		PtrToMemFn _memberFunction;
	};

	// Class to help extend the functionality of generic functor class //

	/*
	template <class FunctorType, typename BoundType, typename... Args>
	class BinderFirst 
		: public FunctorImpl<typename FunctorType::Result, Args...> {

		typedef typename FunctorType::Result ResultType;

	public:

		BinderFirst(FunctorType functor, BoundType value)
			: _functor(functor), _boundValue(value) {

		}

		ResultType operator()(Args... args) override {

			return _functor(_boundValue, std::forward<Args>(args)...);
		}

		FunctorImpl<ResultType, Args...>* clone() const override {

			return new BinderFirst<FunctorType, BoundType, Args...>(_functor, _boundValue);
		}

	private:
		FunctorType _functor;
		BoundType _boundValue;
	};
	*/

	// Template helper functions to use classes that extend Generic functor //

	// Functor class function definitions //

	template <typename ResultType, typename... Args>
	Functor<ResultType, Args...>::Functor()
		: _spImpl(nullptr) {
	}

	template <typename ResultType, typename... Args>
	Functor<ResultType, Args...>::Functor(const Functor& source)
		: _spImpl(nullptr) {

		if (source._spImpl) {
			// source has non-null pointer, clone it and take ownership
			_spImpl = std::unique_ptr<Impl>(source._spImpl->clone());
		}
	}

	template <typename ResultType, typename... Args>
	Functor<ResultType, Args...>& Functor<ResultType, Args...>::operator=(const Functor& rhs) {

		_spImpl.reset(); // Clear contents (calls deleter)

		if (rhs._spImpl) {
			// rhs has non-null pointer, clone it and take ownership
			_spImpl = std::unique_ptr<Impl>(rhs._spImpl->clone());
		}

		return *this;
	}

	template <typename ResultType, typename... Args>
	Functor<ResultType, Args...>::Functor(std::unique_ptr<Impl> spImpl)
		: _spImpl(nullptr) {

		if (spImpl) {
			// unique pointer has non-null pointer, clone it and take ownership
			_spImpl = std::unique_ptr<Impl>(spImpl->clone());
		}
	}

	template <typename ResultType, typename... Args>
	template <typename Funct>
	Functor<ResultType, Args...>::Functor(const Funct& funct) {

		typedef FunctorHandler<Functor<ResultType, Args...>, Funct, Args...> FunctHandler;
		_spImpl = std::unique_ptr<Impl>(new FunctHandler(funct));
	}

	template <typename ResultType, typename... Args>
	template <typename ObjectPtr, typename PtrToMemFn>
	Functor<ResultType, Args...>::Functor(ObjectPtr obj, PtrToMemFn fn) {

		typedef MemberFunctionHandler<Functor<ResultType, Args...>, ObjectPtr, PtrToMemFn, Args...> MemFnHandler;
		_spImpl = std::unique_ptr<Impl>(new MemFnHandler(obj, fn));
	}

	/*
	template <typename ResultType, typename... Args>
	template <typename BoundType>
	void Functor<ResultType, Args...>::BindFirst(BoundType bindValue) {

		//static_assert(Conversion<BoundType, typename PeelFirst<Args>::FirstType>::exists, "Type of boundValue cannot convert to first template parameter");

		// On construction this creates a copy of the functor current functors pImpl object
		// inside the BinderFirst Implementation. This means we can replace the current Impl
		// with the one just allocated safely since the old one is contained inside this one
		Impl* ptrImpl = new BinderFirst<Functor<ResultType, Args...>, BoundType, Args...>(*this, bindValue);
		_spImpl = std::unique_ptr<Impl>(ptrImpl);
	}
	*/

	// Functor Impl function definitions //

	// Handler function definitions //

}