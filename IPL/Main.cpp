#include "stdafx.h"
#include <string.h>
#include <conio.h>
#include "main.h"
#include "Parser.h"

void print(CState *state, Stack *stack, const unsigned param)
{
	TVariant* a;

	//printf(" \"");
	for (unsigned n = 0; n < param; n++)
	{
		a = stack->pop();
		//state->printval(state, a);
		switch (a->type)
		{
		case V_VAR: printf("v_var"); break;
		case V_INT: printf("%d", a->as.i); break;
		case V_FLOAT: printf("%f", a->as.f); break;
		case V_STRING: printf("%.*s", state->constReg.at(a->as.i).as.s[0], state->constReg.at(a->as.i).as.s + 1); break;
		}
		//if(n<param-1) printf(", ");
	}

	//printf("\"");
}

#include <map>
#include <iostream>

float Q_sqrt(float number) 
{
	union {
		int i;
		float y;
	} val;
	float x2;
	const float threehalfs=1.5f;
	x2 = number * 0.5f;
	val.y = number;
	val.i = 0x5f3759df - (val.i >> 1);		//what the fuck
	val.y = val.y * (threehalfs - (x2 * val.y * val.y));	 //1st iteration
	//y = y * (threehalfs - (x2 * y * y));	 //2st iteration, can be removed
	return val.y;
}

namespace CORE {
	char* itoa(int value, char* result, int base) {
		// check that the base if valid
		if (base < 2 || base > 36) { *result = '\0'; return result; }

		char* ptr = result, * ptr1 = result, tmp_char;
		int tmp_value;

		do {
			tmp_value = value;
			value /= base;
			*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * base)];
		} while (value);

		// Apply negative sign
		if (tmp_value < 0) *ptr++ = '-';
		*ptr-- = '\0';
		while (ptr1 < ptr) {
			tmp_char = *ptr;
			*ptr-- = *ptr1;
			*ptr1++ = tmp_char;
		}
		return result;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF | 
		_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_ALLOC_MEM_DF);
//	const char* s = R"(
//	class x {
//		a = 0;
//		fun foo()
//		{
//			print("hello");
//		}
//	}
//	x.foo();
//)";
	const char* s = R"(	
		fun foo()
		{
			print("hello world!");
		}
		x = foo;
		x();
	)";
	/*
	const char* s = R"(
	fun foo(count)
	{
		for i=0 to count
			print(",", i);
		endfor;
	}
	foo(3);
)";*/
//	CORE::itoa(325, temp, 10);
//	return 0;
	CState state;
	CParser parser(&state);
	CVM vm;
	
	state.init();
	state.addCFunction("print", print);
	
	if (parser.parse(s)) 
	{
		printf("\nExecuting code");
		printf("\n======================================================\n");
		vm.run(&state);
	}
	
	return 0;
}
