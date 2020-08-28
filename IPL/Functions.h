#pragma once
#include <vector>
#include "types.h"
#include "CodGen.h"
#include "stack.h"
struct CState;

//pointer to a Cfuntion
typedef void (*c_fun)(CState* state, Stack* stack, const unsigned param);

struct Function;

class TObjectDef
{
public:
	TObjectDef* parent;							//pointer to the parent
	std::vector<TDes> vars;						//register local variables info so when the function is called it can know how to create the variables on the frame stack
	TDes des;									//description info for this object
public:
	std::vector<word> localFunIndex;		//structure need a compile time only

	int findFunction(CState *state, const TString* s);
	int findFunctionRecursive(CState* state, const TString* name);
};

//one unify function that share information for native function or C function
struct Function : public TObjectDef
{
public:
	union FunctionPointer
	{
		c_fun cfun;						//Cfuntion pointer
		CCodeGen* bytecode;				//Native function
	} pfun;								//pointer to a C function or the bytecode for function

	byte param;							//num of parametes that this function can accept
	//functionsIndex localFunIndex;		//structure need a compile time only
	std::vector<short> returns;		//labels to all returns of a function
public:
	Function(VarType type, c_fun data);
	void dispose();
	int findVars(const TString& t) const;
};