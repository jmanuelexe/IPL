#pragma once
#include "types.h"

//althought this is a stack, I need ramdon acces to the elements
//This is because the local variable of every function are store in this stack
class Stack
{
public:

	TVariant* top,					//top of the stack this should be obvious
		*sp;						//stack pointer, this mark the beggin of the current function start of the stack
	TVariant data[100];				//this is the data of the stack
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

