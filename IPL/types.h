#pragma once
#define DEBUG
//typedef unsigned int       UINT;
typedef unsigned int UINT;
typedef unsigned char byte;
typedef unsigned short word;
typedef float NUMBER;

#define INT_SIZE   4
#define FLOAT_SIZE 4
#define WORD_SIZE	2

#define AS_WORD(ip) *((word*)(ip))
#define AS_FLOAT(ip) *((NUMBER*)(ip))
#define AS_BOOL(ip) *((bool*)(ip))
#define AS_INT(ip) *(int*)(ip)
#define AS_SHORT(ip) *((short*)(ip));

#define READ_WORD(ip) *((word*)(ip)); ip+=2;
#define READ_FLOAT(ip) *((NUMBER*)(ip)); ip+=4;
#define READ_BOOL(ip) *((bool*)(ip)); ip++;
#define READ_INT(ip) *(int*)(ip); ip+=4;

#define NOTFOUND -1000000
#define NONE	-1000001


//runtime error
#define ERRORS							99
#define WARNING_MISSING_SEMICOLON		100
#define DIVISION_BY_ZERO				101

//run time error warning
#define INVALID_OPERATOR				1
#define UNKNOWN_INSTRUCTION				2
#define ADD_ERROR(op) case op: return #op

static const char* GETERROR(int code)
{
	switch (code) {
		ADD_ERROR(DIVISION_BY_ZERO);
		ADD_ERROR(UNKNOWN_INSTRUCTION);
		ADD_ERROR(INVALID_OPERATOR);
	default:return "unrecognize Error"; break;
	}
	return "";
}


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

class Array;
struct Function;

typedef union TValue
{
	NUMBER f;						//floating number
	int i;							//integer
	TData* pData;					//pointer to data
	bool b;							//boolean value
	Array* array;					//experimental for array
	char* s;
	Function* fun;
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