#pragma once

#include <typeinfo>

#include "TypeTraits.h"

// Typelists
template <class T, class U>
struct Typelist
{
	typedef T Head;
	typedef U Tail;
};

namespace TL {

	template <class TList>
	struct Length {};

	template <>
	struct Length<NullType> {
		enum { value = 0 };
	};

	template <class T, class U>
	struct Length<Typelist<T, U>> {
		enum { value = 1 + Length<U>::value };
	};


	template <class TList, unsigned int index>
	struct TypeAt {};

	template <class Head, class Tail>
	struct TypeAt<Typelist<Head, Tail>, 0> {
		typedef Head Result;
	};

	template <class Head, class Tail, unsigned int i>
	struct TypeAt<Typelist<Head, Tail>, i> {
		typedef typename TypeAt<Tail, i - 1>::Result Result;
	};


	template <class TList, unsigned int index, typename DefaultType>
	struct TypeAtNonStrict {};

	template <class Head, class Tail, typename DefaultType>
	struct TypeAtNonStrict<Typelist<Head, Tail>, 0, DefaultType> {
		typedef Head Result;
	};

	template <unsigned int i, typename DefaultType>
	struct TypeAtNonStrict<NullType, i, DefaultType> {
		typedef DefaultType Result;
	};

	template <class Head, class Tail, unsigned int i, typename DefaultType>
	struct TypeAtNonStrict<Typelist<Head, Tail>, i, DefaultType> {
		typedef typename TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
	};



	template <class TList, class T>
	struct IndexOf {};

	template <class T>
	struct IndexOf<NullType, T> {
		enum { value = -1 };
	};

	template <class T, class Tail>
	struct IndexOf<Typelist<T, Tail>, T> {
		enum { value = 0; };
	};

	template <class Head, class Tail, class T>
	struct IndexOf<Typelist<Head, Tail>, T>
	{
	private:
		enum { temp = IndexOf<Tail, T>::value };
	public:
		enum { value = temp == -1 ? -1 : 1 + temp };
	};

	// Append to typelist
	template <class TList, class T>
	struct Append {};

	template <>
	struct Append<NullType, NullType> {
		typedef NullType Result;
	};

	template <class T>
	struct Append<NullType, T> {
		typedef Typelist<T, NullType> Result;
	};

	template <class Head, class Tail>
	struct Append<NullType, Typelist<Head, Tail>> {
		typedef Typelist<Head, Tail> Result;
	};

	template <class Head, class Tail, class T>
	struct Append<Typelist<Head, Tail>, T>
	{
		typedef Typelist<Head, typename Append<Tail, T>::Result> Result;
	};

	// Erase from typelist
	template <class TList, class T>
	struct Erase {};

	template <class T>
	struct Erase<NullType, T> {
		typedef NullType Result;
	};

	template <class Head, class Tail, class T>
	struct Erase<Typelist<Head, Tail>, T> {
		typedef Typelist<Head, typename Erase<Tail, T>::Result> Result;
	};

	template <class Tail, class T>
	struct Erase<Typelist<T, Tail>, T> {
		typedef Tail Result;
	};

	// Erase all from typelist
	template <class TList, class T>
	struct EraseAll {};

	template <class T>
	struct EraseAll<NullType, T> {
		typedef NullType Result;
	};

	template <class Head, class Tail, class T>
	struct EraseAll<Typelist<Head, Tail>, T> {
		typedef Typelist<Head, typename EraseAll<Tail, T>::Result> Result;
	};

	template <class Tail, class T>
	struct EraseAll<Typelist<T, Tail>, T> {
		typedef typename EraseAll<Tail, T>::Result Result;
	};

	// NoDuplicates from typelists
	template <class TList>
	struct NoDuplicates {};

	template <>
	struct NoDuplicates<NullType> {
		typedef NullType Result;
	};

	template <class Head, class Tail>
	struct NoDuplicates<Typelist<Head, Tail>> {
	private:
		typedef typename NoDuplicates<Tail>::Result L1;
		typedef typename Erase<L1, Head>::Result L2;
	public:
		typedef Typelist<Head, L2> Result;
	};

	// Replace type in typelist with another type
	template <class TList, class T, class U>
	struct Replace {};

	template <class T, class U>
	struct Replace<NullType, T, U> {
		typedef NullType Result;
	};

	template <class Tail, class T, class U>
	struct Replace<Typelist<T, Tail>, T, U> {
		typedef Typelist<U, Tail> Result;
	};

	template <class Head, class Tail, class T, class U>
	struct Replace<Typelist<Head, Tail>, T, U> {
		typedef Typelist<Head, typename Replace<Tail, T, U>::Result> Result;
	};

} // namespace TL

#define TYPELIST_1(T1) Typelist<T1, NullType>
#define TYPELIST_2(T1, T2) Typelist<T1, TYPELIST_1(T2) >
#define TYPELIST_3(T1, T2, T3) Typelist<T1, TYPELIST_2(T2, T3) >
#define TYPELIST_4(T1, T2, T3, T4) Typelist<T1, TYPELIST_3(T2, T3, T4) >
#define TYPELIST_5(T1, T2, T3, T4, T5) Typelist<T1, TYPELIST_4(T2, T3, T4, T5 ) >
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) Typelist<T1, TYPELIST_5(T2, T3, T4, T5, T6) >

typedef TYPELIST_4(signed char, short, int, long) SignedIntegrals;

// v Straight up magic v //

// I think this concept would be really useful for managing filters/dsps across a project
// as well as having some applications into the editor for all those things and other various classes
template <class TList, template <class> class Unit>
class GenScatterHierarchy;

template <class T1, class T2, template <class> class Unit>
class GenScatterHierarchy<Typelist<T1, T2>, Unit>
	: public GenScatterHierarchy<T1, Unit>, public GenScatterHierarchy<T2, Unit> {
	
public:
	typedef Typelist<T1, T2> TList;
	typedef GenScatterHierarchy<T1, Unit> LeftBase;
	typedef GenScatterHierarchy<T2, Unit> RightBase;

	template <typename T>
	struct Rebind {
		typedef Unit<T> Result;
	};
};

template <class AtomicType, template <class> class Unit>
class GenScatterHierarchy : public Unit<AtomicType> {

	typedef Unit<AtomicType> LeftBase;
	template <typename T> 
	struct Rebind {
		typedef Unit<T> Result;
	};
};

template <template <class> class Unit>
class GenScatterHierarchy<NullType, Unit> {

	template <typename T> 
	struct Rebind {
		typedef Unit<T> Result;
	};

};

template <class T, class H>
typename H::template Rebind<T>::Result& Field(H& obj) {

	return obj;
}

template <class T, class H>
const typename H::template Rebind<T>::Result& Field(const H& obj) {

	return obj;
}

template <class H, unsigned int i>
struct FieldHelper;

template <class H>
struct FieldHelper<H, 0> {

	typedef typename H::TList::Head ElementType;
	typedef typename H::template Rebind<ElementType>::Result UnitType;

	enum { isConst = TypeTraits<H>::isConst };

	typedef const typename H::LeftBase ConstLeftBase;

	typedef typename Select<isConst, ConstLeftBase, typename H::LeftBase>::Result LeftBase;

	typedef UnitType UnqualifiedResultType;

	typedef typename Select<isConst, const UnqualifiedResultType, UnqualifiedResultType>::Result ResultType;

	static ResultType& Do(H& obj) {
		LeftBase& leftBase = obj;
		return leftBase;
	}
};

template <class H, unsigned int i>
struct FieldHelper {

	typedef typename TL::TypeAt<typename H::TList, i>::Result ElementType;
	typedef typename H::template Rebind<ElementType>::Result UnitType;

	enum { isConst = TypeTraits<H>::isConst };

	typedef const typename H::RightBase ConstRightBase;

	typedef typename Select<isConst, ConstRightBase, typename H::RightBase>::Result RightBase;

	typedef	UnitType UnqualifiedResultType;

	typedef typename Select<isConst, const UnqualifiedResultType, UnqualifiedResultType>::Result ResultType;

	static ResultType& Do(H& obj) {
		RightBase& rightBase = obj;
		return FieldHelper<RightBase, i - 1>::Do(rightBase);
	}
};

template <int i, class H>
typename FieldHelper<H, i>::ResultType& Field(H& obj) {
	return FieldHelper<H, i>::Do(obj);
}
