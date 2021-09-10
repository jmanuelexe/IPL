#include "vm.h"


#ifdef WIN32
char s[40];
#include <windows.h>
double get_time()
{
	LARGE_INTEGER t, f;
	QueryPerformanceCounter(&t);
	QueryPerformanceFrequency(&f);
	return (double)t.QuadPart / (double)f.QuadPart;
}
#else
#include <sys/time.h>
#include <sys/resource.h>

double get_time()
{
	struct timeval t;
	struct timezone tzp;
	gettimeofday(&t, &tzp);
	return t.tv_sec + t.tv_usec * 1e-6;
}

#endif

/************************************************

**************************************************/
int strEq(const TString* str1, const TString* str2)
{
	const char* s1, * s2;

	if (!str1 || !str2 || (str1->length != str2->length)) return 0;
	s1 = str1->begin;
	s2 = str2->begin;

	while (*s1 == *s2 && s1 < str1->end) 
	{
		s1++; s2++;
	}

	if (s2 == str2->end) return 1;
	return 0;
}

void printStr(const TString* text)
{
	printf("%.*s", text->length, text->begin);
}

//print a variant
void CVM::printval(CState *state, TVariant* x)
{
	switch (x->type)
	{
	case V_NULL: printf("null"); break;
	case V_FUNCT:
		//printf("%.*s()", state->framestack.stack[x->as.i].des.name.length, state->framestack.stack[x->as.i].des.name.begin); 
		printf("f(%d)", x->as.i);
		break;
	case V_ARRAY:
		printf("{");
		for (word i = 0; i < x->as.array->count - 1; i++) {
			printval(state, &x->as.array->vars[i]);
			printf(",");
		}
		printval(state, &x->as.array->vars[x->as.array->count - 1]);
		printf("}");
		break;
	case V_VAR: printf("%f", stack.sp[x->as.i].as.f); break;
	case V_FLOAT: printf("%2.2f", x->as.f); break;
	case V_BOOL: printf((x->as.b) ? "true" : "false"); break;
	case V_INT: printf("%d", x->as.i); break;
	case V_STRING: printf("%.*s", state->constReg.at(x->as.i).as.s[0], state->constReg.at(x->as.i).as.s + 1); break;
	}
}
//***************************************************************************
//
//
//***************************************************************************

#ifdef DEBUG
#define PUSHVAR(index)\
	printf("%.*s", fun->vars[index].name.length, fun->vars[index].name.begin);\
	printf("%-12s", " ");\
	stack.pushvar(stack.sp[index]);
#else
#define PUSHVAR(index) stack.pushvar(stack.sp[index]);
#endif // DEBUG


#define POP_A_AND_B_THEN b = stack.pop(); a = stack.pop();
#define INVALIDMATHOPERANDS {state->setError(INVALID_OPERATOR);stack.pushb(0);}
#define STR_CONCAT_THEN if (a->type == V_STRING) {char s[80];\
if (b->type == V_STRING) {\
	sprintf(s, "%s%s", constants.at(a->as.i).as.s, constants.at(b->as.i).s);\
	stack.pushString(str.add(s));\
}else if (b->type == V_INT) {\
	sprintf(s, "%s%d", str.str[a->as.i], b->as.i);\
	stack.pushString(str.add(s));\
}else if (b->type == V_FLOAT) {\
	sprintf(s, "%s%f", str.str[a->as.i], b->as.f);\
	stack.pushString(str.add(s));\
}\
} else if (b->type == V_STRING) {\
	char s[80];\
	if (a->type == V_INT) {\
		sprintf(s, "%d%s", a->as.i, str.str[b->as.i]);\
		stack.pushString(str.add(s));\
	}else if (a->type == V_FLOAT) {\
		sprintf(s, "%1.4f%s", a->as.f, str.str[b->as.i]);\
		stack.pushString(str.add(s));\
	}\
}else\

#define MATH_OP(a, op, b) \
if (a->type == V_FLOAT) {\
if (b->type == V_INT) stack.pushf(a->as.f op b->as.i);\
else if (b->type == V_FLOAT) stack.pushf(a->as.f op b->as.f);\
else INVALIDMATHOPERANDS\
}else if (a->type == V_INT) {\
	if (b->type == V_FLOAT) stack.pushf(a->as.i op b->as.f);\
	else if (b->type == V_INT) stack.pushi(a->as.i op b->as.i);\
	else INVALIDMATHOPERANDS\
}else INVALIDMATHOPERANDS

#define DO_LOGIC_OP(a, op, b) \
if (a->type == V_FLOAT) {\
if (b->type == V_INT) stack.pushb(a->as.f op b->as.i);\
else if (b->type == V_FLOAT) stack.pushb(a->as.f op b->as.f);\
else INVALIDMATHOPERANDS\
}else if (a->type == V_INT) {\
	if (b->type == V_FLOAT) stack.pushb(a->as.i op b->as.f);\
	else if (b->type == V_INT) stack.pushb(a->as.i op b->as.i);\
	else INVALIDMATHOPERANDS\
}else INVALIDMATHOPERANDS

/*********************************************************************************
		Virtual machine definition

*********************************************************************************/
CVM::~CVM()
{
	
}


NUMBER CVM::doPower(NUMBER a, NUMBER b)
{
	NUMBER r;
	word i;
	r = a;
	for (i = 1; i < b; i++)
		r *= a;
	return r;
}

void CVM::storeA(CState * state, Function* fun, int varindex)
{
	TVariant *var  = &stack.sp[varindex];
	TVariant* val  = stack.pop();
	int arrayIndex = stack.pop()->as.i;

	if(var->type != V_ARRAY) 
	{
		state->arrayReg.emplace_back();
		state->deleteVal(var);
		var->type = V_ARRAY;
		var->as.array = &state->arrayReg.back();
		var->as.array->ref++;
	}

	if (arrayIndex >= var->as.array->count) {
		var->as.array->resize(arrayIndex+1);
	}

	var->as.array->vars[arrayIndex].as = val->as;
	var->as.array->vars[arrayIndex].type = val->type;
#ifdef DEBUG
	sprintf(s, "%.*s[%d]", fun->vars[varindex].name.length, fun->vars[varindex].name.begin, arrayIndex);
	printf("%-15s", s);
	//printval(state, var);
#endif
}

void CVM::assing(CState* state, TVariant* source, TVariant *destination)
{
	if (source->type == V_ARRAY) source->as.array->ref++;
	if (destination->type == V_ARRAY) destination->as.array->ref--;
	
	destination->type = source->type;
	destination->as = source->as;
}

void CVM::store(CState * state, Function *fun, int index, bool local)
{
	TVariant *source, *destination;

	//We pop the value off the stack and then we mus delete if it reserved memory(so far only array)
	//then we copy it destination on the frame stack
	source = stack.pop();
	destination = &stack.sp[index];
	assing(state, source, destination);

#ifdef DEBUG
	printf("%.*s %-7s", fun->vars[index].name.length, fun->vars[index].name.begin, " ");
	printf("\t");
#endif
}

#define SHOWSTACK \
printf("[ "); TVariant *x;\
for (x = stack.data; x < stack.top-1; x++){ \
	printval(state, x); printf(", ");\
}	if(stack.data!=stack.top) printval(state, x); printf(" ]\n"); 

//#define PUSHLVAR(index) printStr(&fun->vars[index].name); printf("%-14s"," ");\
//			stack.pushvar(stack.sp[5]);
//			//printval(state, &stack.sp[index]);
//
void CVM::_run(CState *state, Function* fun)
{
	word count, index;
	char *cp, * ip = fun->bytecode.get();
	cp = ip;
	TVariant *b, *a, *prvSP = stack.sp;
	OPCODE opcode;
	Function* pFun;

	stack.sp += fun->vars.size();

	if (state->isError()) return;
#ifdef DEBUG
	printf("\n\nRunning function %.*s", fun->des.name.length, fun->des.name.begin);
	printf("\nAddress   OPCODE     Operand        Stack\n");
#endif
	while (*ip != OP_HALT && state->getError()< ERRORS)
	{
		opcode = (OPCODE)*ip;
#ifdef DEBUG
			printf("%p: %-10s ", ip, OPCODESTR(opcode));
#endif
		ip++;
		switch (opcode)
		{
		case OP_SETS:
		{
			index = READ_WORD(ip);
			a = &stack.sp[index];
			state->deleteVal(a);
			a->as.i = READ_WORD(ip);
			a->type = V_STRING;
#ifdef DEBUG
			printStr(&fun->vars[index].name);
			//printf("=%d %-11s", stack.sp[index].as.i, " ");
			printval(state, a);
#endif
		}
		break;
		case OP_SETI: 
		{
			index= READ_WORD(ip);
			a = &stack.sp[index];
			state->deleteVal(a);
			a->as.i = READ_WORD(ip);
			a->type = V_INT;
#ifdef DEBUG
			printStr(&fun->vars[index].name); 
			printf("=%d %-11s", stack.sp[index].as.i, " ");
#endif
		}
			break;
		case OP_SETF:
		{
			index = READ_WORD(ip);
			a = &stack.sp[index];
			state->deleteVal(a);
			a->as.f = READ_FLOAT(ip);
			a->type = V_FLOAT;
#ifdef DEBUG
			printStr(&fun->vars[index].name); 
			printf("=%f ", stack.sp[index].as.f);
#endif
		}break;
		case OP_SETB:
		{
			index = READ_WORD(ip);
			a = &stack.sp[index];
			state->deleteVal(a);
			a->as.b = READ_BOOL(ip); ip ++;
			a->type = V_BOOL;
#ifdef DEBUG
			printStr(&fun->vars[index].name);
			printf("=%d ", stack.sp[index].as.i);
#endif
		}		break;
		case OP_PUSHF: stack.pushFunc(AS_WORD(ip)); ip += WORD_SIZE;	break;
		case OP_PUSHI: stack.pushi(AS_INT(ip));	ip += INT_SIZE;	break;
		case OP_PUSH0: stack.pushi(0);break;
		case OP_PUSH1: stack.pushi(1);break;
		case OP_PUSH2: stack.pushi(2);break;
		case OP_PUSH3: stack.pushi(3);break;
		case OP_PUSH4: stack.pushi(4);break;
		case OP_PUSH5: stack.pushi(5);break;
		case OP_PUSHB: stack.pushb(AS_BOOL(ip));	ip++;		break;
		case OP_PUSHV0	:PUSHVAR(0);  break;
		case OP_PUSHV1	:PUSHVAR(1); break;
		case OP_PUSHV2	:PUSHVAR(2); break;
		case OP_PUSHV3	:PUSHVAR(3); break;
		case OP_PUSHV4	:PUSHVAR(4); break;
		case OP_PUSHV5	:PUSHVAR(5);  break;
		case OP_PUSHV	:stack.pushvar(stack.sp[AS_WORD(ip)]); ip += WORD_SIZE;	break;
		case OP_PUSHS	:stack.pushString(AS_WORD(ip)); ip += WORD_SIZE; break;//add the index of the string constant
		case OP_PUSHVA	:
			a = stack.pop();
#ifdef DEBUG
			printStr(&fun->vars[AS_WORD(ip)].name);
			sprintf(s,"[%d]", a->as.i);
			printf("%-15s",s);
#endif
			stack.pushvar(stack.sp[AS_WORD(ip)].as.array->vars[a->as.i]);
			ip += WORD_SIZE;
			break;
		case OP_INCI: stack.sp[AS_WORD(ip)].as.i++; ip += WORD_SIZE;break;
		case OP_INCF: stack.sp[AS_WORD(ip)].as.f++; ip += WORD_SIZE;break;
		case OP_DECI: stack.sp[AS_WORD(ip)].as.i--; ip += WORD_SIZE; break;
		case OP_DECF: stack.sp[AS_WORD(ip)].as.f--; ip += WORD_SIZE; break;
		case OP_STOREVA:storeA(state, fun, AS_WORD(ip)); ip += WORD_SIZE;break;
		case OP_STOREV1:store(state, fun, 1, true); break;
		case OP_STOREV0:store(state, fun, 0, true); break;
		case OP_STOREV2:store(state, fun, 2, true); break;
		case OP_STOREV3:store(state, fun, 3, true); break;
		case OP_STOREV4:store(state, fun, 4, true); break;
		case OP_STOREV5:store(state, fun, 5, true); break;
		case OP_STOREV :store(state, fun, AS_WORD(ip), true); ip += WORD_SIZE; break;
		case OP_STOREF: {
			TVariant* source, * destination;
			destination = &stack.sp[AS_WORD(ip)];
			ip += WORD_SIZE;
			destination->as.fun = state->funReg[AS_WORD(ip)];
			destination->type = V_FUNCT;
			ip += WORD_SIZE;
			//store(state, fun, AS_WORD(ip), true); ip += WORD_SIZE; 
		}
			break;
		case OP_PLUS:
			POP_A_AND_B_THEN
			//STR_CONCAT_THEN
			MATH_OP(a, +, b)
			break;
		case OP_POWER:
			POP_A_AND_B_THEN
			stack.pushf(doPower(b->as.f, a->as.f));
			break;
		case OP_MINUS:	POP_A_AND_B_THEN MATH_OP(a, -, b)	break;
		case OP_MULT:	POP_A_AND_B_THEN MATH_OP(a, *, b)	break;
		case OP_DIV:	POP_A_AND_B_THEN 
			if (b->type == V_FLOAT) {
				if (b->as.f == 0.0f) { stack.pushf(0.0f); state->setError(DIVISION_BY_ZERO); }
				else if (a->type == V_INT) stack.pushf(a->as.f / b->as.i); 
				else if (a->type == V_FLOAT) stack.pushf(a->as.f / b->as.f);
				else INVALIDMATHOPERANDS
			}else if (b->type == V_INT) {
					if (b->as.i == 0) { 
						stack.pushf(0); state->setError(DIVISION_BY_ZERO); 
					}
				else if (a->type == V_FLOAT) stack.pushf(float(a->as.i) / b->as.f);
				else if (a->type == V_INT) stack.pushf(float(a->as.i) / float(b->as.i)); 
				else INVALIDMATHOPERANDS
			}
			else INVALIDMATHOPERANDS

			break;
		case OP_GT:	POP_A_AND_B_THEN DO_LOGIC_OP(a, > , b)		break;
		case OP_GE:	POP_A_AND_B_THEN DO_LOGIC_OP(a, >= , b)	break;
		case OP_LT:	POP_A_AND_B_THEN DO_LOGIC_OP(a, < , b)		break;
		case OP_LE:	POP_A_AND_B_THEN DO_LOGIC_OP(a, <= , b)	break;
		case OP_AND:POP_A_AND_B_THEN DO_LOGIC_OP(a, &&, b)		break;
		case OP_OR:	POP_A_AND_B_THEN DO_LOGIC_OP(a, || , b)	break;
		case OP_EQ:
			POP_A_AND_B_THEN
			if (a->type == V_STRING && b->type == V_STRING) 
				stack.pushb( strcmp(state->constReg.at(a->as.i).as.s,
					state->constReg.at(b->as.i).as.s)==0);
			else {
				if (a->type == V_FLOAT) {
					if (b->type == V_INT) stack.pushb(a->as.f==b->as.i);
					else if (b->type == V_FLOAT) stack.pushb(a->as.f==b->as.f);
					else INVALIDMATHOPERANDS
				} else if (a->type == V_INT) {
					if (b->type == V_FLOAT) stack.pushb(a->as.i==b->as.f);
					else if (b->type == V_INT) stack.pushb(a->as.i==b->as.i);
					else INVALIDMATHOPERANDS
				}
				else INVALIDMATHOPERANDS
				//DO_LOGIC_OP(a, == , b)
			}
			break;
		case OP_JMZ:
			b = stack.pop();
			if (b->as.b == 0) {
				ip = cp + (short)AS_WORD(ip); //if false jump
			} else
				ip +=WORD_SIZE;	//else skip label(2bytes) and go to next instruction
#ifdef DEBUG
			printf(" jump to %p\t", ip);
#endif
			break;
		case OP_JMP : 
			ip = cp +AS_WORD(ip);
#ifdef DEBUG
			printf("jump to %p\t", ip);
#endif
			break;
		case OP_CALL:
		case OP_ICALL:
			index = READ_WORD(ip);	//function index
			count = READ_WORD(ip);	//Count of parameter
			if (opcode == OP_ICALL)
			{
				TVariant* source = &stack.sp[index];
				index = source->as.i;
				if (source->type != V_FUNCT) 
					break;
			}
#ifdef DEBUG
			printStr(&state->funReg[index]->des.name);
#endif
				//TVariant* source = &stack.sp[vindex];
				//index = source->as.i;			//call normal function
				if (state->funReg[index]->des.type == V_FUNCT)
				{
					pFun = state->funReg[index];
					_run(state, pFun);
					//restore sp
					stack.sp = prvSP;
				}//call a C function
				else if (state->funReg[index]->des.type == V_CFUNCT) {
					state->funReg[index]->cfun(state, &stack, count);
#ifdef DEBUG
					printf("%-6s", " ");
#endif
				}			
			break;
		case OP_EXIT: 
#ifdef DEBUG
			printf("%-15s", " ");		
#endif
			return;
		default:
			printf("unrecognized instrunction code : %d", opcode);
			state->setError(UNKNOWN_INSTRUCTION);
			return;
		}

#ifdef DEBUG
		
		printf("[ "); TVariant* x;
		for (x = stack.data; x < stack.top - 1; x++) 
		{
			printval(state, x); printf(", ");
		}

		if (stack.data != stack.top) 
			printval(state, x); printf(" ]\n");
#endif
	}
#ifdef DEBUG
	printf("\n%p: %-10s ", ip, OPCODESTR((OPCODE)*ip));
#endif
  }

//5030 broadway suite 707
void CVM::run(CState *state)
{
	_run(state, state->global);
	if (state->getError()>ERRORS)
	{
		printf("Runtime error :%s\n", GETERROR(state->getError()));
	}
}