#pragma once

#include "Typelists.h"

#include <memory>

template <typename ReturnType, class TList>
class FunctorImpl;

template <typename ReturnType, class TList>
class Functor {

public:
	typedef FunctorImpl<ReturnType, TList> Impl;

	typedef TList ParmList;
	typedef ReturnType ResultType;
	typedef typename TL::TypeAtNonStrict<TList, 0, EmptyType>::Result Parm1;
	typedef typename TL::TypeAtNonStrict<TList, 1, EmptyType>::Result Parm2;
	// ...

	Functor();
	Functor(const Functor&);

	template <class Fun>
	Functor(Fun functor);

	Functor& operator=(const Functor&);

	explicit Functor(std::auto_ptr<Impl> spImpl);
	
	ReturnType operator()() {

		return (*_spImpl)();
	}

	ReturnType operator()(Parm1 p1) {

		return (*_spImpl)(p1);
	}

	ReturnType operator()(Parm1 p1, Parm2 p2) {

		return (*_spImpl)(p1, p2);
	}

private:
	std::auto_ptr<Impl> _spImpl;
};



template <typename ReturnType>
class FunctorImpl<ReturnType, NullType> {

public:
	virtual ReturnType operator()() = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {};

};

template <typename ReturnType, typename P1>
class FunctorImpl<ReturnType, TYPELIST_1(P1)> {

public:
	virtual ReturnType operator()(P1) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {};

};

template <typename ReturnType, typename P1, typename P2>
class FunctorImpl<ReturnType, TYPELIST_2(P1, P2)> {

public:
	virtual ReturnType operator()(P1, P2) = 0;
	virtual FunctorImpl* Clone() const = 0;
	virtual ~FunctorImpl() {};

};

template <class ParentFunctor, typename Fun>
class FunctorHandler : public FunctorImpl
	<typename ParentFunctor::ResultType, typename ParentFunctor::ParmList> {

public:
	typedef typename ParentFunctor::ResultType ResultType;

	FunctorHandler(Fun functor) : _functor(functor) {}
	//FunctorHandler(const Fun& functor) : _functor(functor) {}

	FunctorHandler* Clone() const {

		return new FunctorHandler(*this);
	}

	ResultType operator()() {

		return _functor();
	}
	ResultType operator()(typename ParentFunctor::Parm1 p1) {

		return _functor(p1);
	}
	ResultType operator()(typename ParentFunctor::Parm1 p1, 
						  typename ParentFunctor::Parm2 p2) {

		return _functor(p1, p2);
	}

private:
	Fun _functor;

};

/*
template <class ParentFunctor, typename PtrToObj, typename PtrToMemFn>
class MemberFnHandler : public FunctorImpl
	<typename ParentFunctor::ResultType, typename ParentFunctor::ParmList> {

public:
	typedef typename ParentFunctor::ResultType ResultType;

	MemberFnHandler(const PtrToObject& pObj, PtrToMemFn pMemFn)
		: _pObj(pObj), _pMemFn(pObj) {}

	MemberFnHandler* Clone() const {

		return new MemberFnHandler(*this);
	}
	
	ResultType operator()() {

		return ((*_pObj).*_pMemFn)();
	}
	ResultType operator()(typename ParentFunctor::Parm1 p1) {
		
		return ((*_pObj).*_pMemFn)(p1);
	}
	ResultType operator()(typename ParentFunctor::Parm1 p1, typename ParentFunctor::Parm2 p2) {

		return ((*_pObj).*_pMemFn)(p1, p2);
	}

private:
	PtrToObj _pObj;
	PtrToMemFn _pMemFn;
};
*/

template <typename ResultType, class TList>
template <typename Fun>
Functor<ResultType, TList>::Functor(Fun functor)
	: _spImpl(new FunctorHandler<Functor, Fun>(functor)) { }

/*
template <typename ResultType, class TList>
template <typename Fun>
Functor<ResultType, TList>::Functor(const Fun& functor)
	: _spImpl(new FunctorHandler<Functor, Fun>(functor)) { }
*/