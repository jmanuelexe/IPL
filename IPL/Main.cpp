#include "stdafx.h"
#include <string.h>
#include <conio.h>
#include "main.h"
#include "Parser.h"

void print(CState *state, Stack *stack, const unsigned param)
{
	TVariant* a;

	printf(" \"");
	for (unsigned n = 0; n < param; n++)
	{
		a = stack->pop();
		//state->printval(state, a);
		switch (a->type)
		{
		case V_VAR: printf("v_var"); break;
		case V_INT: printf("%d", a->as.i); break;
		case V_FLOAT: printf("%f", a->as.f); break;
		}
		if(n<param-1) printf(", ");
	}

	printf("\"");
}


#ifdef TYPED
#include <string>
#include <iostream>
#endif
int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF | 
		_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_ALLOC_MEM_DF);
	
	const char* s = R"(
fun power(a, b)
{
	r=1;
	for c=1 to b
		r = r * a;
	endfor;
	return r;
}
	
	print(power(2,5));
)";
	CState state;
	CParser parser(&state);
	CVM vm;
	
	state.init();

	state.addCFunction("print", print);
	
//#define TYPED
#ifdef  TYPED
	std::string source;

	std::getline(std::cin, source);
	if (!parser.parse(source.c_str())) {
#else
	if (parser.parse(s)) 
	{
#endif
		printf("\nExecuting code");
		printf("\n======================================================");
		vm.run(&state);
	}

	return 0;
}
