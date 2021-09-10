#pragma once
#include "types.h"
#include "Functions.h"
#include "Array.h"
#include <memory>

typedef std::vector<TVariant> VarList;
typedef std::vector<Function*> FucList;
typedef std::vector<Array> ArrayList;

struct VarsReg {
	VarList stack;						//variables stack
	TVariant* sp;						//pointer to the stack
};

struct CState
{
	friend class CVM;
	friend class CParser;
	friend class TObjectDef;
private:
	//Stack stack;						//for stack operation
	int	m_error;						//store error (parsing, compiling)
	std::vector<TObject> objReg;		//registry of objects
	FucList funReg;						//Function Registry
	ArrayList arrayReg;					//Array Registry
	//VarsReg framestack;					//frame stack locals variables
	Function *global;					//Native function
public:
	VarList constReg;					//constant Registry(so far String)
public:
	~CState();
	void destroy();
	void init();
	int addCFunction(const char *name, c_fun);
	int createSymbol(TObjectDef *parent, const TString& name, const VarType type, c_fun data);
	void setError(int er) {
		m_error = er; 
	}
	int getError() { return m_error; }
	void deleteVal(TVariant* val);
	int isError();
	int findSymbol(const TObjectDef* fun, const TString& name, bool& local) const;
};