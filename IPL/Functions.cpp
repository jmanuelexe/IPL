#include "Functions.h"
#include "State.h"

/*********************************************************************************
*
*
***********************************************************************************/

//search a function in this Object if not found it will reurn NOFOUND
int TObjectDef::findFunction(CState* state, const TString* s)
{
	for (int v = 0; v < (int)localFunIndex.size(); v++)
	{
		if (strEq(&state->funReg[localFunIndex[v]]->des.name, s))
			return localFunIndex[v];
	}
	return NOFOUND;
}

/***********************************************************************************
*
************************************************************************************/

Function::Function(VarType type, c_fun data)
{
	if (type == V_FUNCT)
		pfun.bytecode = new CCodeGen();
	else
		pfun.cfun = data;

	des.type = V_NULL;
	parent = 0;
	param = 0;
}

//dispose its oject
void Function::dispose()
{
	if (des.type == V_FUNCT)
	{
		if (pfun.bytecode)
		{
			delete pfun.bytecode;
			pfun.bytecode = 0;
		}
	}
}

//find a function in this Object if not found it will check on its parent and up until it reaches the Main (root) object reurn NOFOUND
int TObjectDef::findFunctionRecursive(CState* state, const TString* name)
{
	TObjectDef* pCurFun = this;
	int fIndex = NONE;

	while (fIndex == NONE && pCurFun)
	{
		fIndex = pCurFun->findFunction(state, name);
		if (fIndex != NONE)
			return fIndex;
		else {
			pCurFun = pCurFun->parent;
		}
	}

	return fIndex;
}

//finds a variable in this class
int Function::findVars(const TString& t) const
{
	for (int index = 0; index < (int)vars.size(); index++)
	{
		if (strEq(&vars[index].name, &t))
			return index;
	}
	return NOFOUND;
}
