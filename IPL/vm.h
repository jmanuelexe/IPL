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
public:
	TStack stack;		//for stack operation
	~CVM();
	static void printval(CState* state, TVariant* x);
	NUMBER doPower(NUMBER a, NUMBER b);
	void run(CState* state);
	void storeA(CState* state, TFunction* fun, const int varIndex);
	void assing(CState* state, TVariant* source, TVariant* destination);
	void store(CState* state, TFunction* fun, int index, bool local);
	void _run(CState* state, TFunction* fun);
};

#endif