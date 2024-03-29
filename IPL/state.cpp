#include "State.h"

/**********************************************************************************************
			CState Class definition
**********************************************************************************************/
void CState::init()
{
	TString name;

	m_error = 0;
	global = 0;

	name = makeString("Main");
	int index = createSymbol(0, name, V_FUNCT, NULL);

	if (index != NOTFOUND)
		global = funReg[index];
}

int CState::addCFunction(const char* FunName, c_fun fun)
{
	TString name;
	
	name = makeString(FunName);
	
	int findex;// , vindex;

	findex = createSymbol(global, name, V_CFUNCT, fun);
	//vindex = createSymbol(global, name, V_VAR, fun);
	//vindex = global.addVar(name, V_VAR, createSymbol(global, name, V_VAR, 0));
	//global->findVars(name);
	//global->bytecode.emit1w(OP_PUSHF, findex);
	//global->bytecode.emit1w(OP_STOREV, vindex);
	//return createSymbol(global, name, V_CFUNCT, fun);
	return findex;
}

CState::~CState()
{
	destroy();
}

void CState::destroy()
{
	unsigned n;

	for (n = 0; n < funReg.size(); n++)
	{
		funReg[n]->dispose();
		delete funReg[n];
	}

	for (n = 0; n < constReg.size(); n++)
	{
		if (constReg[n].type == V_STRING) {
			delete[] constReg[n].as.s;
		}
	}

	for (n = 0; n < arrayReg.size(); n++)
	{
		arrayReg[n].dispose();
	}

	//remove resources
	arrayReg.clear();
	funReg.clear();
	constReg.clear();

}

int CState::isError()
{

	return	m_error;
}

//find symbol in the scope or deeper if not found on the active scope
int CState::findSymbol(const TObjectDef* fun, const TString& name, bool& local) const
{
	int index = NOTFOUND;
	const TObjectDef* pCurFun = fun;
	static int stepback = 0;
	int i = 0;
	local = true;
	
	while (index < 0 && pCurFun)
	{
		index = pCurFun->findVars(name);
		if (index != NOTFOUND)
		{
			return index;
		}else {
			local = false;
			pCurFun = pCurFun->parent;
		}
	}
	return NOTFOUND;
}

// Creates a variable in the "parent" or create function(C or native)
// add it to the "parent" function scope if parent = null
int CState::createSymbol(TObjectDef* parent, const TString& name, const VarType type, c_fun data)
{
	int index = NONE;
	TDes des;
	TVariant var;

	des.name.begin = name.begin;
	des.name.end = name.end;
	des.name.length = name.length;

	switch (type)
	{
	case V_CFUNCT:
	case V_FUNCT:
	case V_CLASS:
	{
		des.type = type;
		Function* o = new Function(type, data);
		o->des = des;

		funReg.push_back(o);
		index = (int)(funReg.size() - 1);

		if (parent) {
			o->parent = parent;
			index = (int)funReg.size() - 1;
			parent->localFunIndex.push_back(index);
		}

#ifdef DEBUG
		printf("Function '%.*s' added.\n", name.length, name.begin);
#endif // DEBUG
	}
	break;
	case V_ARRAY:
	case V_VAR:
		des.type = V_NULL;// V_INT;
		parent->vars.push_back(des);
		index = (int)parent->vars.size() - 1;
#ifdef DEBUG
		printf("Var '%.*s'index(%d) added in function %.*s.\n", name.length, name.begin, index,
			parent->des.name.length, parent->des.name.begin);
#endif // DEBUG

		break;
	}

	return index;
}

void CState::deleteVal(TVariant* val)
{
	switch (val->type) {
	case V_ARRAY:
		printf("\nDeleting array");
		val->type = V_NULL;
		if (val->as.array)
		{
			delete val->as.array;
			val->as.array = 0;
		}
	default: break;
	}
}