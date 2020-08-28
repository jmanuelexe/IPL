#pragma once
#include "types.h"
#include "Mem.h"

#define MAXBUF 255
enum OPCODE
{
	OP_HALT=-1,	//stop the program
	OP_EXIT,	//exit a function
	OP_PUSHC,	//stack = Constant
	OP_PUSHC0,	//stack = 0
	OP_PUSHC1,	//stack = 1
	OP_PUSHC2,	//stack = 2
	OP_PUSHC3,	//stack = 3
	OP_PUSHV,	//stack = local var[n]
	OP_PUSHV0,	//stack = local var[0]
	OP_PUSHV1,	//stack = local var[1]
	OP_PUSHV2,	//stack = local var[2]
	OP_PUSHV3,	//stack = local var[3]
	OP_PUSHV4,	//stack = local var[3]
	OP_PUSHV5,	//stack = local var[3]

	OP_PUSHF,	//stack = float number
	OP_PUSHI,	//stack = int
	OP_PUSH0,	//stack = 0
	OP_PUSH1,	//stack = 1
	OP_PUSH2,	//stack = 2
	OP_PUSH3,	//stack = 3
	OP_PUSH4,	//stack = 4
	OP_PUSH5,	//stack = 5
	OP_PUSHS,	//stack = "string"
	OP_PUSHB,	//stack = boolean
	
	OP_PUSHVA,
	OP_POP,
	OP_STOREVA, 
	OP_STOREV,	//local variable[n] = stack
	OP_STOREV0,	//shortcut //local variable[0] = stack
	OP_STOREV1,
	OP_STOREV2,
	OP_STOREV3,
	OP_STOREV4,
	OP_STOREV5,
	OP_SETV,
	OP_SETI,
	OP_SETF,
	OP_SETB,
	OP_SETS,
	OP_AND,		//Boolean and
	OP_OR,		//boolean or
	OP_GT,		//Greater than operator	(>)
	OP_GE,		//less or equal operator(>=)
	OP_LT,		//less than operator	(<)
	OP_LE,		//less or equal operator (<=)
	OP_EQ,		//compare to values and return true if they are equal
	OP_MULT,	//multiplication operator
	OP_ASSIGN,
	OP_DIV,		//division operator
	OP_PLUS,	//addition operator
	OP_MINUS,	//minus operator
	OP_POWER,	//x to the power of operator
	OP_INCI,	//increment a local interger variable
	OP_INCF,	//increment a local float variable
	OP_DECI,	//Decrement a local interger variable
	OP_DECF,	//Decrement a local float variable
	OP_JMZ,		//ump if 0 or false
	OP_JMP,		//unconditional jump (incrementing intrusction pointer)
	OP_CALL		//call a function or C Function
};

#define ADD_OPCODE(op) case op: return #op

static const char* OPCODESTR(int code)
{
	switch (code) {
		ADD_OPCODE(OP_PUSHI);
		ADD_OPCODE(OP_PUSH0);
		ADD_OPCODE(OP_PUSH1);
		ADD_OPCODE(OP_PUSH2);
		ADD_OPCODE(OP_PUSH3);
		ADD_OPCODE(OP_PUSH4);
		ADD_OPCODE(OP_PUSH5);
		ADD_OPCODE(OP_PUSHS);
		ADD_OPCODE(OP_PUSHC);
		ADD_OPCODE(OP_PUSHV0);
		ADD_OPCODE(OP_PUSHV1);
		ADD_OPCODE(OP_PUSHV2);
		ADD_OPCODE(OP_PUSHV3);
		ADD_OPCODE(OP_PUSHV4);
		ADD_OPCODE(OP_PUSHV5);
		ADD_OPCODE(OP_HALT);
		ADD_OPCODE(OP_EXIT);
		ADD_OPCODE(OP_PUSHF);
		ADD_OPCODE(OP_PUSHV);
		ADD_OPCODE(OP_PUSHVA);
		ADD_OPCODE(OP_POP);
		ADD_OPCODE(OP_MULT);
		ADD_OPCODE(OP_DIV);
		ADD_OPCODE(OP_AND);
		ADD_OPCODE(OP_GT);
		ADD_OPCODE(OP_GE);
		ADD_OPCODE(OP_LT);
		ADD_OPCODE(OP_LE);
		ADD_OPCODE(OP_EQ);
		ADD_OPCODE(OP_PLUS);
		ADD_OPCODE(OP_MINUS);
		ADD_OPCODE(OP_OR);
		ADD_OPCODE(OP_JMZ);
		ADD_OPCODE(OP_JMP);
		ADD_OPCODE(OP_CALL);
		ADD_OPCODE(OP_INCI);
		ADD_OPCODE(OP_INCF);
		ADD_OPCODE(OP_DECI);
		ADD_OPCODE(OP_DECF);
		ADD_OPCODE(OP_PUSHC0);
		ADD_OPCODE(OP_PUSHC1);
		ADD_OPCODE(OP_PUSHC2);
		ADD_OPCODE(OP_PUSHC3);
		ADD_OPCODE(OP_STOREVA);
		ADD_OPCODE(OP_STOREV);
		ADD_OPCODE(OP_STOREV0);
		ADD_OPCODE(OP_STOREV1);
		ADD_OPCODE(OP_STOREV2);
		ADD_OPCODE(OP_STOREV3);
		ADD_OPCODE(OP_STOREV4);
		ADD_OPCODE(OP_STOREV5);
		ADD_OPCODE(OP_SETV);		
		ADD_OPCODE(OP_SETI);		
		ADD_OPCODE(OP_SETF);		
		ADD_OPCODE(OP_SETB);		
		ADD_OPCODE(OP_POWER);
		default:return "unrecognize instuction"; break;
	}
	return "";
};



#define GetIndex (unsigned)(cur - start)
#define GROWIFNEEDED(i) if (GetIndex+i >= size) append(i);


class CCodeGen: public TSimpleAlloc
{
	word offset;
public:
	bool fin;
public:
	CCodeGen();
	CCodeGen(const int size);
	void append(int size);
	void emitN(NUMBER num);
	void emitW(word num);
	//void emitMathOp(OPCODE code, byte a, byte b, byte c);
	void emiti(int num);
	void emitBool(byte opcode, bool val);
	void emitB(byte opcode);
	void emit(OPCODE opcode);
	void emit1f(OPCODE opcode, NUMBER num);
	void emit1i(OPCODE opcode, int num);
	void emit1i1f(OPCODE opcode, word parm1, NUMBER parm2);
	void emit2w(OPCODE opcode, word parm1, word parm2);
	void emit1w(OPCODE opcode, word num);
	void emit1mb(OPCODE opcode, const char* str, const int size);
	void EmitBWithPromt(OPCODE op);
	word getCurPos();
	void emitjump(OPCODE opcode, short jumpOffset);
	word beginjump(OPCODE opcode);
	void endJump(word label);
	void print();
};

