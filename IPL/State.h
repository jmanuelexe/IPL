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
	int	m_error;						//store error (parsing, compiling)
	FucList funReg;						//Function Registry
	ArrayList arrayReg;					//Array Registry
	VarsReg framestack;					//frame stack locals variables
	VarList constReg;					//constant Registry(so far String)
	Function *global;					//Native function
public:
	~CState();
	void destroy();
	void init();
	int addCFunction(const char *name, c_fun);
	int createSymbol(Function*parent, const TString& name, const VarType type, c_fun data);
	void setError(int er) {
		m_error = er; 
	}
	int getError() { return m_error; }
	void deleteVal(TVariant* val);
	void printval(TVariant* x);
	int isError();
	int findSymbol(const Function* fun, const TString& name, bool& local) const;
};