#include "Stack.h"
#include <stdio.h>
#include <memory.h>
Stack::Stack()
{
	memset(data, 0, sizeof(data));
	top = data;
	sp = frame;
}

void Stack::pushf(NUMBER num) {
#ifdef DEBUG
	printf("%2.2f ", num);
#endif // DEBUG
	top->as.f = num;
	top->type = V_FLOAT;
	top++;
}

//V_BYTE, V_WORD, V_INT
void Stack::pushi(int val) {
#ifdef DEBUG
	printf("%-15d", val);
#endif // DEBUG
	top->as.i = val;
	top->type = V_INT;
	top++;
}

void Stack::pushb(bool val) {
#ifdef DEBUG
	printf((val) ? "true" : "false");
#endif // DEBUG
	top->as.b = val;
	top->type = V_BOOL;
	top++;
}

//index is theinde on the constant vextor where all the string reside
void Stack::pushString(word index) {
	top->as.i = index;
	top->type = V_STRING;
	/*if (p >= data + 100) {
		printf("stack overflow");return;
	}*/
	top++;
}

void Stack::pushvar(TVariant var) {
	if (top >= data + 100) {
		printf("stack overflow");
		return;
	}
	*top++ = var;
}

void Stack::pushFunc(word index) {
	top->as.i = index;
	top->type = V_FUNCT;
	if (top >= data + 100) {
		printf("stack overflow");
		return;
	}
	top++;
}

TVariant* Stack::pop() {
	if (top > data) return --top;
	else {
		printf("stack underflow");
		return 0;
	}
}

