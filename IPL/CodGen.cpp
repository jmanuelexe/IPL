#include "CodGen.h"
#include <stdlib.h>
#include <memory>
//
//CCodeGen::CCodeGen(const CCodeGen & copy)
//{
//	bufsize = 0;
//	buffer = 0;
//	pcode = temp;
//	fin = 0;
//}

CCodeGen::CCodeGen()
{
	offset = 0;
	start=0;
	fin = false; 
}

CCodeGen::CCodeGen(const int size):TSimpleAlloc()
{
	resize(size);
	offset = 0;
	fin = false;
}

/*
it will resize the buffer to fit size bytes need after pcode
	ex if the requested 2 and pcode is 0 and the buffer is 1 it will only add one byte
*/	
void CCodeGen::append(int bytes_needed)
{
	unsigned expected = offset + bytes_needed;
	
	if (expected > getSize()) {
		resize(expected);
	}
}

void CCodeGen::emitB(byte opcode)
{
	if (fin) return;
	append(1);
	start[offset++] = opcode;
}

void CCodeGen::emitBool(byte opcode, bool val)
{
	if (fin) return;
	emitB(opcode);
	emitB(val);
}

void CCodeGen::emitW(word num)
{
	if (fin) return;
	append(WORD_SIZE);
	*(word*)(start+offset) = num;
	offset += WORD_SIZE;
}
//
//void CCodeGen::emitMathOp(OPCODE code, byte a, byte b, byte c) 
//{
//	int i;
//	i = (int)code;
//	i << 6;// = a;
//}

void CCodeGen::emiti(int num)
{
	if (fin) return;
	append(INT_SIZE);
	*(int*)(start + offset) = num;
	offset += INT_SIZE;
}

void CCodeGen::emitN(NUMBER num)
{
	if (fin) return;
	append(FLOAT_SIZE);
	*(NUMBER*)(start + offset) = num;
	offset += FLOAT_SIZE;
}

#define TRIMEBUFFER

void CCodeGen::emit(OPCODE opcode)
{
#ifdef DEBUG
	printf("%5d: %s\n", offset, OPCODESTR(opcode));
#endif
	emitB((byte)opcode);
	if (opcode == OP_HALT/* || opcode == OP_EXIT*/) {
		fin = 1;
		TRIMEBUFFER
	}

}

void CCodeGen::emit1f(OPCODE opcode, NUMBER num)
{
#ifdef DEBUG
	printf("%5d: %s\t %f\n", offset, OPCODESTR(opcode), num);
#endif
	emitB(opcode);
	emitN(num);
}

//will write an opcode, series of char(not a zero terminate) and it's len
void CCodeGen::emit1mb(OPCODE opcode, const char* str, const int len)
{
	if (fin) return;
#ifdef DEBUG
	printf("%5d: %s\t \"%s\"\n", offset, OPCODESTR(opcode), str);
#endif
	emitB(opcode);
	emitW(len);
	
	append(len);

	for (int i = 0; i < len; i++)
		*(start+offset++)= str[i];
}

void CCodeGen::emit1w(OPCODE opcode, word num)
{
	switch (opcode)
	{
	case OP_STOREV:
		switch (num)
		{
		case 0:	emit(OP_STOREV0);	return;
		case 1:	emit(OP_STOREV1);	return;
		case 2:	emit(OP_STOREV2);	return;
		case 3: emit(OP_STOREV3);	return;
		case 4: emit(OP_STOREV4);	return;
		case 5: emit(OP_STOREV5);	return;
		}
	}
#ifdef DEBUG
	printf("%5d: %s\t %d\n", offset, OPCODESTR(opcode), num);
#endif
	emitB(opcode);
	emitW(num);
}

void CCodeGen::EmitBWithPromt(OPCODE op) {
#ifdef DEBUG 
	printf("%5d: %s\n", offset, OPCODESTR(op));
#endif
	emitB(op);
}

void CCodeGen::emit1i(OPCODE opcode, int num)
{ 
	switch (opcode) 
	{
	case OP_PUSHI:
		switch (num)
		{			
		case 0:EmitBWithPromt(OP_PUSH0); return;
		case 1:EmitBWithPromt(OP_PUSH1); return;
		case 2:EmitBWithPromt(OP_PUSH2); return;
		case 3:EmitBWithPromt(OP_PUSH3); return;
		case 4:EmitBWithPromt(OP_PUSH4); return;
		case 5:EmitBWithPromt(OP_PUSH5); return;
		}break;
	case OP_STOREV: 
		switch (num) {
		case 0: EmitBWithPromt(OP_STOREV0); return;
		case 1: EmitBWithPromt(OP_STOREV1); return;
		case 2: EmitBWithPromt(OP_STOREV2); return;
		}break;
	case OP_PUSHV:
		switch (num) {
		case 0: EmitBWithPromt(OP_PUSHV0); return;
		case 1: EmitBWithPromt(OP_PUSHV1); return;
		case 2: EmitBWithPromt(OP_PUSHV2); return;
		case 3: EmitBWithPromt(OP_PUSHV3); return;
		case 4: EmitBWithPromt(OP_PUSHV4); return;
		case 5: EmitBWithPromt(OP_PUSHV5); return;
		}break;
	}

#ifdef DEBUG
	printf("%5d: %s\t %d\n", offset, OPCODESTR(opcode), num);
#endif
	emitB(opcode);

	emiti(num);
}

void CCodeGen::emit1i1f(OPCODE opcode, word parm1, NUMBER parm2) 
{
#ifdef DEBUG
	printf("%5d: %s\t %d, %f\n", offset, OPCODESTR(opcode), parm1, parm2);
#endif
	emitB(opcode);
	emitW(parm1);
	emitN(parm2);
}

void CCodeGen::emit2w(OPCODE opcode, word parm1, word parm2)
{
#ifdef DEBUG
	printf("%5d: %s\t %d, %d\n", offset, OPCODESTR(opcode), parm1, parm2);
#endif
	emitB(opcode);
	emitW(parm1);
	emitW(parm2);
}

void CCodeGen::print()
{
	for (unsigned n = 0; n < size; n++) 
		printf("%d ", start[n]);
}

void CCodeGen::emitjump(OPCODE opcode, short jumpOffset)
{
#ifdef DEBUG
	printf("%5d: %s\n", offset, OPCODESTR(opcode));
#endif // DEBUG

	emitB(opcode);
	*(short*)(start+offset) = jumpOffset;
	offset += 2;
}

//write the jump byte code and also 2 bytes to save the amount of byte to jump to
word CCodeGen::beginjump(OPCODE opcode)
{
	word plabel;
#ifdef DEBUG
	printf("%5d: %s ", offset, OPCODESTR(opcode));
#endif // DEBUG
	emitB(opcode);
	append(WORD_SIZE);
	*(word*)(start+offset) = 0;
	plabel =  offset;
	offset += WORD_SIZE;
#ifdef DEBUG
	printf("at %5d\n", plabel);
#endif // DEBUG

	return plabel;
}

//update the location name by 'label' to the amount of bytes already writen
void CCodeGen::endJump(word label)
{
#ifdef DEBUG
	printf("updating label %d=%d\n", label, offset);
#endif
	*(short*)(start + label) = offset;
}

word CCodeGen::getCurPos()
{
	return offset;
}

