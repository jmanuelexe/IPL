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

#define NOFOUND -1000000
#define NONE	-1000001

//errors
#define ERROR_NONE						0
#define ERROR_MISSING_OPERATOR			300
#define ERROR_MISSING_CLOSE_PARENTHESIS	301
#define ERROR_MISSING_CLOSE_BRACKET		302
#define ERROR_MISSING_SEMICOLON			303
#define ERROR_NUMER_EXPECTED			304
#define ERROR_INVALID_STRING			305
#define ERROR_INVALID_INDENTFIER		306
#define ERROR_MISSING_END				307
#define ERROR_MISSING_FUNCTIONNAME		308
#define ERROR_MISSING_BEGIN				309
#define ERROR_MISSING_THEM				310
#define ERROR_MISSING_CONDITION			311
#define ERROR_MISSING_COLON				312
#define ERROR_MISSING_BREAK				313
#define ERROR_MISSING_SWITCH			314
#define ERROR_MISSING_STATEMENT			315
#define ERROR_SPECTED_VAR				316
#define ERROR_MISSING_LABEL				317

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
