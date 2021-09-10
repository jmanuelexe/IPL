#pragma once
#include <vector>
#include "types.h"
#include "CodGen.h"
#include "stack.h"
struct CState;

//pointer to a Cfuntion
typedef void (*c_fun)(CState* state, Stack* stack, const unsigned param);

struct Function;

//representation of a object in memory
struct TObject {
	std::vector<TVariant> vars;
};

//Descriction of a variable
struct TDes
{
	TString name;	//ID that points to the name of the variable to avoid repetition
	VarType type;	//type of object
};

class TObjectDef
{
public:
	TObject *instance;							//pointer to the active instance
	CCodeGen bytecode;							//Native function
	TObjectDef* parent;							//pointer to the parent
	std::vector<TDes> vars;						//register local variables info so when the function is called it can know how to create the variables on the frame stack
	TDes des;									//description info for this object
public:
	std::vector<word> localFunIndex;			//structure need a compile time only

	int findFunction(CState *state, const TString* s);
	int findFunctionRecursive(CState* state, const TString* name);
	int findVars(const TString& t) const;
};

//one unify function that share information for native function or C function
struct Function : public TObjectDef
{
public:
	c_fun cfun;							//Cfuntion pointer
	byte param;							//num of parametes that this function can accept
	//functionsIndex localFunIndex;		//structure need a compile time only
	std::vector<short> returns;			//labels to all returns of a function
public:
	Function(VarType type, c_fun data);
	void dispose();
};