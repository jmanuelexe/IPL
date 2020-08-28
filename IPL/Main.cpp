#include "stdafx.h"
#include <string.h>
#include <conio.h>
#include "main.h"
#include "Parser.h"

void print(CState *state, TStack *stack, const unsigned param)
{
	TVariant* a;

	printf("(");
	for (unsigned n = 0; n < param-1; n++)
	{
		a = stack->pop();
		CVM::printval(state, a);
		printf(", ");
	}

	a = stack->pop();
	CVM::printval(state, a);
	printf(") ");
}


int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF | 
		_CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_ALLOC_MEM_DF);
	
	//if (v >= 1 && v <= 2)
	const char* s = R"(
function foo(v)
	{
		print(v);
	}
	v[0] = 10;
	v[1] = 20;
	foo(v);
)";

	CState state;
	CParser parser(&state);
	CVM vm;
	
	state.init();

	TString strPrint;
	
	strPrint = makeString("print");
	state.createSymbol(state.global, strPrint, V_CFUNCT, print);
	
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
		printf("\n======================================================\n");
		vm.run(&state);
	}

	return 0;
}
