#pragma once

class NullType {};

class EmptyType {};

template <int v>
struct Int2Type {
	enum { value = v };
};

template <class T>
struct Type2Type {
	typedef T OriginalType;
};

template <bool flag, typename T, typename U>
struct Select {
	typedef T Result;
};

template <typename T, typename U>
struct Select<false, T, U> {
	typedef U Result;
};

template <class T, class U>
class Conversion
{
private:
	typedef char Small;
	class Big { public: char dummy[2]; };

	static Small Test(U);
	static Big Test(...);
	static T MakeT();

public:
	enum { exists = sizeof(Test(MakeT())) == sizeof(Small), sameType = false };
};

template <class T>
class Conversion<T, T>
{
public:
	enum { exists = true, sameType = true };
};

#define SUPERSUBCLASS(T, U) \
	(Conversion<const U*, const T*>::exists && \
	 !Conversion<const T*, const void*>::sameType)

#define SUPERSUBCLASS_STRICT(T, U) \
	(SUPERSUBCLASS(T, U) && !Conversion<const T, const U>::sameType)

template <typename T>
class TypeTraits {

private:
	template <class U>
	struct PointerTraits {
		enum { result = false };
		typedef NullType PointeeType;
	};

	template <class U>
	struct PointerTraits<U*> {
		enum { result = true };
		typedef U PointeeType;
	};

	template <class U>
	struct ReferenceTraits {
		enum { result = false };
		typedef NullType ReferencedType;
	};

	template <class U>
	struct ReferenceTraits<U&> {
		enum { result = true };
		typedef U ReferencedType;
	};

	template <class U> 
	struct memFnPtrTraits {
		enum { result = false };
	};

	template <class U, class V>
	struct memFnPtrTraits<U V::*> {
		enum { result = true };
	};

	template <class U>
	struct ConstTraits {
		enum { result = false };
		typedef NullType ConstType;
	};

	template <class U>
	struct ConstTraits<const U> {
		enum { result = true };
		typedef U ConstType;
	};

public:
	enum {
		isPointer = PointerTraits<T>::result,
		isReference = ReferenceTraits<T>::result,
		isMemFnPointer = memFnPtrTraits<T>::result,
		isConst = ConstTraits<T>::result,
	};

	typedef typename PointerTraits<T>::PointeeType PointeeType;
	typedef typename ReferenceTraits<T>::ReferencedType ReferencedType;
	typedef typename ConstTraits<T>::ConstType ConstantType;
};