#pragma once
#include "types.h"
#include "CodGen.h"

typedef struct TString {
	const char* begin, * end;		//store the text found in the sourse
	int  length;			//lenth
}TString;

int strEq(const TString* str1, const TString* str2);
void printStr(const TString* text);
TString makeString(const char* strz);

//the type of variable
typedef enum : unsigned char
{
	//value type
	V_NULL,		//0
	V_struct,	//1
	V_FLOAT,	//2
	V_BOOL,		//4
	V_INT,		//8
	V_STRING,	//16

	//object type
	V_CONST,	//32
	V_VAR,		//64
	V_PTR,		//128
	V_ARRAY,	//256
	V_FUNCT,	//512
	V_CFUNCT,	//1024
	V_CLASS
}VarType;

//store constant
struct TData {
	byte ref;	//reference counting
	char* p;	//a pointer to the data
	byte size;	//size of bytes
};

struct TStack;
struct CState;

//pointer to a Cfuntion
typedef void (*c_fun)(CState* state, TStack* stack, const unsigned param);

//Descriction of a variable
struct TDes
{
	word len;	//size for str or array
	VarType type;	//type of vatiable see above
	TString name;	//ID that points to the name of the variable to avoid repetition
};

struct TVariant;	//forward definition of a variant
struct TFunction;
struct Array;
struct TObject;

typedef union TValue
{
	NUMBER f;						//floating number
	int i;							//integer
	TData* pData;					//pointer to data
	bool b;							//boolean value
	Array* array;					//experimental for array
	char* s;
	TFunction* fun;
	TObject *object;
}TValue;

//the representation of a variable
typedef struct TVariant
{
public:
	TValue as;
	VarType type;	//type of vatiable see above
	TVariant() {
		as.i = 0;
		type = V_NULL;
	}
	
	TVariant(TValue val, VarType t) 
	{ 
		as = val; type = t;
	}
}TVariant;

typedef std::vector<TVariant> VarList;

struct TObject
{
	TVariant self;
	TVariant* vars;
	word count;
public:
	TObject() { count = 0; vars = 0; }
	//bool resize(const int CountElements);
	//void dispose();
};

struct Array {
	TVariant* vars;
	word count;
public:
	Array() { count = 0; vars = 0; }
	bool resize(const int CountElements);
	void dispose();
};


struct TObjectDef
{
public:
	TObjectDef* parent;					//pointer to the parent
	std::vector<TDes> vars;			//register local variables info so when the function is called it can know how to create the variables on the frame stack
	TDes des;							//description info for this object
public:
	TFunction *curFun;
	std::vector<word> localFunIndex;		//structure need a compile time only

	int findFunction(CState* state, const TString* s);
	int findFunctionRecursive(CState* state, const TString* name);
};

//one unify function that share information for native function or C function
struct TFunction : public TObjectDef
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
	TFunction(VarType type, c_fun data);
	void dispose();
	int findVars(const TString* t) const;
};

struct TStack
{
public:
	TVariant* p;
	TVariant data[100];
public:
	TStack();
	void pushf(NUMBER num);
	void pushi(int val);
	void pushb(bool val);
	void pushString(word index);
	void pushvar(TVariant var);
	void pushFunc(word index);
	TVariant* pop();
};

typedef std::vector<TFunction*> TFucList;
typedef std::vector<TObjectDef*> TObjectList;

struct VarsReg {
	VarList stack;						//variables stack
	TVariant* sp;						//pointer to the frame stack
};

typedef struct TParent {
	union {
		TFunction* fun;
		TObjectDef* clas;
	}as;
	VarType type;
}TParent;

struct CState
{
	int	m_error;						//store error (parsing, compiling)
public:
	TObjectList objects;					//classes
	TFucList functions;					//Registry all functions
	VarList constants;					//Registry all constants
	VarsReg framestack;					//frame stack locals variables
	TFunction *global;					//Native function
	//TParent* global;
public:
	~CState();
	void destroy();
	void init();
	int createSymbol(TFunction*parent, const TString* name, const VarType type, c_fun data);
	void setError(int er) {
		m_error = er; 
	}
	int getError() { return m_error; }
	void deleteVal(TVariant* val);
	int isError();
	int findSymbol(const TFunction* fun, const TString* name, bool& local) const;
};