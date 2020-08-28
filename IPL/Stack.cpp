#include "Stack.h"
#include <stdio.h>
#include <memory.h>
Stack::Stack()
{
	memset(data, 0, sizeof(data));
	p = data;
}

void Stack::pushf(NUMBER num) {
#ifdef DEBUG
	printf("%2.2f ", num);
#endif // DEBUG
	p->as.f = num;
	p->type = V_FLOAT;
	p++;
}

//V_BYTE, V_WORD, V_INT
void Stack::pushi(int val) {
#ifdef DEBUG
	printf("%-15d", val);
#endif // DEBUG
	p->as.i = val;
	p->type = V_INT;
	p++;
}

void Stack::pushb(bool val) {
#ifdef DEBUG
	printf((val) ? "true" : "false");
#endif // DEBUG
	p->as.b = val;
	p->type = V_BOOL;
	p++;
}

//index is theinde on the constant vextor where all the string reside
void Stack::pushString(word index) {
	p->as.i = index;
	p->type = V_STRING;
	/*if (p >= data + 100) {
		printf("stack overflow");return;
	}*/
	p++;
}

void Stack::pushvar(TVariant var) {
	if (p >= data + 100) {
		printf("stack overflow");
		return;
	}
	*p++ = var;
}

void Stack::pushFunc(word index) {
	p->as.i = index;
	p->type = V_FUNCT;
	if (p >= data + 100) {
		printf("stack overflow");
		return;
	}
	p++;
}

TVariant* Stack::pop() {
	if (p > data) return --p;
	else {
		printf("stack underflow");
		return 0;
	}
}

