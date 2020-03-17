#pragma once

#include "TemplateGarbage/Typelists.h"
#include "TemplateGarbage/TypeTraits.h"

#include <iostream>

typedef TYPELIST_6(char, short, int, short, int, int) integerList;
typedef TYPELIST_2(float, double) floatingList;

typedef TYPELIST_3(char, int, short) basicInts;

typedef TL::Append<integerList, long>::Result AllIntegers;
typedef TL::Append<integerList, floatingList>::Result AllTypes;

typedef TL::NoDuplicates<integerList>::Result UniqueIntegers;

typedef TL::Replace<UniqueIntegers, short, float>::Result bit16;

class Widget {};

class Button : public Widget {};

class SpecialButton : public Button {};

class Slider : public Widget {};

typedef TYPELIST_4(Button, Slider, Widget, SpecialButton) WidgetFamily;

// TypeTraits tom-foolery

class foo {
public:
	foo(int value) :
		_value(value) {}

	foo(const foo& source) :
		_value(source._value) {}

	int Read() { return _value; }

private:
	int _value;
};

class base {
public:
	base(int value) :
		_value(value) {}

	base(const base& source) = delete;

	virtual base* Clone() {
		return new base(_value);
	}

	virtual int Read() { return _value; }

private:
	int _value;
};

class derived : public base {
public:
	derived(int value) :
		base(value - 1), _value(value) {}

	derived(const derived& source) = delete;

	virtual base* Clone() {
		return new derived(_value);
	}

	virtual int Read() { return _value; }

private:
	int _value;
};

template <typename T, bool isPolymorphic>
class NiftyContainer {
public:

	NiftyContainer() : heldObject(nullptr) {}

	void DoSomething(T* pSomeObject) {
		DoSomething(pSomeObject, Int2Type<isPolymorphic>());
	}

	void PrintContents() {
		if (heldObject) {
			std::cout << heldObject->Read() << std::endl;
		}
	}

private:
	void DoSomething(T* pSomeObject, Int2Type<true>) {
		T* pNewObject = pSomeObject->Clone();
		heldObject = pNewObject;
	}

	void DoSomething(T* pSomeObject, Int2Type<false>) {
		T* pNewObject = new T(*pSomeObject);
		heldObject = pNewObject;
	}

	T* heldObject;
};

template <class T>
struct Holder {
	T value_;
};

typedef GenScatterHierarchy<TYPELIST_3(int, std::string, Widget), Holder> WidgetInfo;