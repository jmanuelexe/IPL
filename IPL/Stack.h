#pragma once
#include "types.h"

class Stack
{
public:
	TVariant* p;
	TVariant data[100];
public:
	Stack();
	void pushf(NUMBER num);
	void pushi(int val);
	void pushb(bool val);
	void pushString(word index);
	void pushvar(TVariant var);
	void pushFunc(word index);
	TVariant* pop();
};

