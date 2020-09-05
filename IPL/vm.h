#ifndef VM_H
#define VM_H
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <conio.h>
#include <stdio.h>
#include "state.h"
//#include "CodGen.h"
double get_time();

class CVM
{
	Stack stack;		//for stack operation
public:
	~CVM();
	void run(CState* state);
private:
	void storeA(CState* state, Function* fun, const int varIndex);
	void assing(CState* state, TVariant* source, TVariant* destination);
	void store(CState* state, Function* fun, int index, bool local);	NUMBER doPower(NUMBER a, NUMBER b);
	void _run(CState* state, Function* fun);
	void printval(CState* state, TVariant* x);
};

#endif