#include "vm.h"
#include <stdio.h>


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

//***************************************************************************
//
//
//***************************************************************************
#define POP_OPERANDS_THEN b = stack.pop(); a = stack.pop();
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

/**********************************************************************************************
			Array Class definition
**********************************************************************************************/
TStack::TStack()
{
	memset(data, 0, sizeof(data));
	p = data;
}

void TStack::pushf(NUMBER num) {
#ifdef DEBUG
	printf("%2.2f ", num);
#endif // DEBUG
	p->as.f = num;
	p->type = V_FLOAT;
	p++;
}

//V_BYTE, V_WORD, V_INT
void TStack::pushi(int val) {
#ifdef DEBUG
	printf("%-15d", val);
#endif // DEBUG
	p->as.i = val;
	p->type = V_INT;
	p++;
}

void TStack::pushb(bool val) {
#ifdef DEBUG
	printf((val) ? "true" : "false");
#endif // DEBUG
	p->as.b = val;
	p->type = V_BOOL;
	p++;
}

//index is theinde on the constant vextor where all the string reside
void TStack::pushString(word index) {
	p->as.i = index;
	p->type = V_STRING;
	/*if (p >= data + 100) {
		printf("stack overflow");return;
	}*/
	p++;
}

void TStack::pushvar(TVariant var) {
	if (p >= data + 100) {
		printf("stack overflow");
		return;
	}
	*p++ = var;
}

void TStack::pushFunc(word index) {
	p->as.i = index;
	p->type = V_FUNCT;
	if (p >= data + 100) {
		printf("stack overflow");
		return;
	}
	p++;
}

TVariant* TStack::pop() {
	if (p > data) return --p;
	else {
		printf("stack underflow");
		return 0;
	}
}

/**********************************************************************************************
			Array Class definition
**********************************************************************************************/
//increase /decrease the count of object for array has(not the bytes)
bool Array::resize(const int CountElements)
{
	TVariant* temp;
	word size = CountElements * int(sizeof(TVariant));
	temp = (TVariant*)realloc(vars, size);

	if (temp) {
		vars = temp;
		this->count = CountElements;
		return true;
	}
	return false;
}

void Array::dispose()
{
	if (vars)
	{
		free(vars);
		vars = 0;
	}
}





/**********************************************************************************************
			CState Class definition
**********************************************************************************************/
void CState::init()
{
	TString name;

	m_error = 0;
	global = 0;

	name = makeString("Main");
	int index = createSymbol(0, name, V_FUNCT, NULL);

	if(index != NOFOUND)
		global = functions[index];
}

CState::~CState()
{
	destroy();
}

void CState::destroy()
{
	unsigned n;

	for (n = 0; n < objects.size(); n++)
	{
		//objects[n]->dispose();
		delete objects[n];
	}

	for (n = 0; n < functions.size(); n++) 
	{
		functions[n]->dispose();
		delete functions[n];
	}

	for (n = 0; n < constants.size(); n++)
	{
		if (constants[n].type == V_STRING) {
			delete[] constants[n].as.s;
		}
	}
	
	for (n = 0; n < arrays.size(); n++) 
	{
		arrays[n].dispose();
	}

	//remove resources
	arrays.clear();
	objects.clear();
	functions.clear();
	constants.clear();

}

int CState::isError()
{

	return	m_error;
}

//find symbol in the scope or deeper if not found on the active scope
int CState::findSymbol(const TFunction* fun, const TString& name, bool& local) const
{
	int index = -1;
	const TFunction* pCurFun = fun;
	static int stepback = 0;
	int i = 0;
	local = true;
	index = pCurFun->findVars(name);
	return index;
	/*
	while (index < 0 && pCurFun)
	{
		index = pCurFun->findVars(name);
		if (index != NOFOUND)
		{
			return index;
		}else {
			local = false;
			pCurFun = pCurFun->parent;
		}
	}
	*/
	return NOFOUND;
}

// Creates a variable in the "parent" or create function(C or native)
// add it to the "parent" function scope if parent = null
int CState::createSymbol(TFunction* parent, const TString& name, const VarType type, c_fun data)
{
	int index = NONE;
	TDes des;
	TVariant var;

	des.name.begin  = name.begin;
	des.name.end	= name.end;
	des.name.length = name.length;

	switch (type)
	{
	case V_CFUNCT:
	case V_FUNCT:
	{
		des.type = type;
		TFunction* o = new TFunction(type, data);
		o->des = des;		

		functions.push_back(o);
		index = (int)(functions.size() - 1);

		if (parent) {
			o->parent = parent;
			index = (int)functions.size() - 1;
			parent->localFunIndex.push_back(index);
		}
		
#ifdef DEBUG
		printf("Function '%.*s' added.\n", name.length, name.begin);
#endif // DEBUG
	}
	break;
	case V_ARRAY:
	case V_VAR:
		des.type = V_NULL;// V_INT;
			parent->vars.push_back(des);
			index = (int)parent->vars.size() - 1;
#ifdef DEBUG
			printf("Var '%.*s'index(%d) added in function %.*s.\n", name.length, name.begin, index,
				parent->des.name.length, parent->des.name.begin);
#endif // DEBUG
		
		break;
	}

	return index;
}

void CState::deleteVal(TVariant *val)
{
	switch (val->type) {
	case V_ARRAY:
		printf("\nDeleting array");
		val->type = V_NULL;
		if (val->as.array)
		{
			delete val->as.array;
			val->as.array = 0;
		}
	default: break;
	}
}

/*********************************************************************************
*
*
***********************************************************************************/

//search a function in this Object if not found it will reurn NOFOUND
int TObjectDef::findFunction(CState* state, const TString* s)
{
	for (int v = 0; v < (int)localFunIndex.size(); v++) 
	{
		if (strEq(&state->functions[localFunIndex[v]]->des.name, s)) 
			return localFunIndex[v];
	}
	return NOFOUND;
}

/***********************************************************************************
*
************************************************************************************/

TFunction::TFunction(VarType type, c_fun data)
{
	if (type == V_FUNCT)
		pfun.bytecode = new CCodeGen();
	else 
		pfun.cfun = data;

	des.type = V_NULL;
	parent = 0;
	param = 0;
}

//dispose its oject
void TFunction::dispose() 
{
	if (des.type == V_FUNCT) 
	{
		if (pfun.bytecode) 
		{
			delete pfun.bytecode;
			pfun.bytecode = 0;
		}
	}
}

//find a function in this Object if not found it will check on its parent and up until it reaches the Main (root) object reurn NOFOUND
int TObjectDef::findFunctionRecursive(CState* state, const TString* name)
{
	TObjectDef* pCurFun = this;
	int fIndex = NONE;

	while (fIndex == NONE && pCurFun)
	{
		fIndex = pCurFun->findFunction(state, name);
		if (fIndex != NONE)
			return fIndex;
		else {
			pCurFun = pCurFun->parent;
		}
	}

	return fIndex;
}

//finds a variable in this class
int TFunction::findVars(const TString& t) const
{
	for (int index = 0; index < (int)vars.size(); index++) 
	{
		if (strEq(&vars[index].name, &t))
			return index;
	}
	return NOFOUND;
}

/*********************************************************************************
		Virtual machine definition

*********************************************************************************/
CVM::~CVM()
{
	
}

//print a variant
void CVM::printval(CState *state, TVariant *x)
{
	switch (x->type)
	{
	case V_NULL: printf("null"); break;
	case V_FUNCT: 
		//printf("%.*s()", state->framestack.stack[x->as.i].des.name.length, state->framestack.stack[x->as.i].des.name.begin); break;
	case V_ARRAY:
		printf("{");
		for (word i = 0; i < x->as.array->count-1; i++){
			printval(state, &x->as.array->vars[i]);
			printf(",");
		}
		printval(state, &x->as.array->vars[x->as.array->count - 1]);
		printf("}\n");
		break;
	case V_VAR	: printf("%f", state->framestack.stack[x->as.i].as.f); break;
	case V_FLOAT: printf("%2.2f", x->as.f); break;
	case V_BOOL	: printf((x->as.b) ? "true" : "false"); break;
	case V_INT: printf("%d", x->as.i); break;
	case V_STRING: 
		printf("%.*s", state->constants.at(x->as.i).as.s[0], state->constants.at(x->as.i).as.s+1); break;
	}
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

void CVM::storeA(CState * state, TFunction* fun, int varindex)
{
	TVariant *var = &state->framestack.sp[varindex];
	TVariant* val = stack.pop();
	int arrayIndex = stack.pop()->as.i;

	if(var->type != V_ARRAY) 
	{
		state->arrays.emplace_back();
		state->deleteVal(var);
		var->type = V_ARRAY;
		var->as.array = &state->arrays.back();
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

void CVM::store(CState * state, TFunction *fun, int index, bool local)
{
	TVariant *source, *destination;

	//We pop the value off the stack and then we mus delete if it reserved memory(so far only array)
	//then we copy it destination on the frame stack
	source = stack.pop();
	destination = &state->framestack.sp[index];
	assing(state, source, destination);

#ifdef DEBUG
	printf("%-15.*s", fun->vars[index].name.length, fun->vars[index].name.begin);
#endif
}

#define SHOWSTACK \
printf("[ "); TVariant *x;\
for (x = stack.data; x < stack.p-1; x++){ \
	printval(state, x); printf(", ");\
}	if(stack.data!=stack.p) printval(state, x); printf(" ]\n"); 

#define PUSHLVAR(index) printStr(&fun->vars[index].name); printf("%-14s"," ");
			//printval(state, &state->framestack.sp[index]);

void CVM::_run(CState *state, TFunction* fun)
{
	word funIndex, count;
	char* ip = fun->pfun.bytecode->get();
	char* cp = ip;
	TVariant *b, *a;
	OPCODE opcode;

	int index;
	int prevSStackSize = (int)state->framestack.stack.size();// current frame end
	int varcount = (int)fun->vars.size();
	int newFrameStackSize = prevSStackSize + varcount;

	if (state->framestack.stack.size() != newFrameStackSize) {
#ifdef DEBUG
		printf("\nChanging frame stack from %d to %d\n", (int)state->framestack.stack.size(), newFrameStackSize);
#endif
		state->framestack.stack.resize(newFrameStackSize);
	}
	
	if(state->framestack.stack.size() && varcount>0)
		state->framestack.sp = &state->framestack.stack.at(prevSStackSize);

	if (state->isError()) return;
#ifdef DEBUG
	printf("\nAddress           OPCODE     Operand        Stack\n");
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
			a = &state->framestack.sp[index];
			state->deleteVal(a);
			a->as.i = READ_WORD(ip);
			a->type = V_STRING;
#ifdef DEBUG
			printStr(&fun->vars[index].name);
			//printf("=%d %-11s", state->framestack.sp[index].as.i, " ");
			printval(state, a);
#endif
		}
		break;
		case OP_SETI: 
		{
			index= READ_WORD(ip);
			a = &state->framestack.sp[index];
			state->deleteVal(a);
			a->as.i = READ_WORD(ip);
			a->type = V_INT;
#ifdef DEBUG
			printStr(&fun->vars[index].name); 
			printf("=%d %-11s", state->framestack.sp[index].as.i, " ");
#endif
		}
			break;
		case OP_SETF:
		{
			index = READ_WORD(ip);
			a = &state->framestack.sp[index];
			state->deleteVal(a);
			a->as.f = READ_FLOAT(ip);
			a->type = V_FLOAT;
#ifdef DEBUG
			printStr(&fun->vars[index].name); 
			printf("=%f ", state->framestack.sp[index].as.f);
#endif
		}break;
		case OP_SETB:
		{
			index = READ_WORD(ip);
			a = &state->framestack.sp[index];
			state->deleteVal(a);
			a->as.b = READ_BOOL(ip); ip ++;
			a->type = V_BOOL;
#ifdef DEBUG
			printStr(&fun->vars[index].name);
			printf("=%d ", state->framestack.sp[index].as.i);
#endif
		}		break;
		case OP_PUSHI: stack.pushi(AS_INT(ip));	ip += INT_SIZE;	break;
		case OP_PUSH0: stack.pushi(0);break;
		case OP_PUSH1: stack.pushi(1);break;
		case OP_PUSH2: stack.pushi(2);break;
		case OP_PUSH3: stack.pushi(3);break;
		case OP_PUSH4: stack.pushi(4);break;
		case OP_PUSH5: stack.pushi(5);break;
		case OP_PUSHF: stack.pushf(AS_FLOAT(ip));	ip += FLOAT_SIZE;	break;
		case OP_PUSHB: stack.pushb(AS_BOOL(ip));	ip++;		break;
		case OP_PUSHC0: stack.pushi((int)0);	break;
		case OP_PUSHC1: stack.pushi((int)1);	break;
		case OP_PUSHC2: stack.pushi((int)2);	break;
		case OP_PUSHC3: stack.pushi((int)3);	break;
		case OP_PUSHS:
			stack.pushString(AS_WORD(ip));//add the index of the string constant
			ip += WORD_SIZE;	//the indextook to bytes now we must skip them
			break;
		case OP_PUSHV	:
#ifdef DEBUG
			PUSHLVAR(AS_INT(ip))
#endif
			stack.pushvar(state->framestack.sp[AS_INT(ip)] ); ip += INT_SIZE;
			break;
		case OP_PUSHV0:
#ifdef DEBUG
			PUSHLVAR(0)
#endif
			stack.pushvar(state->framestack.sp[0]);
			break;
		case OP_PUSHV1:
#ifdef DEBUG
			PUSHLVAR(1)
#endif
			stack.pushvar(state->framestack.sp[1]);
			break;
		case OP_PUSHV2:
#ifdef DEBUG
			PUSHLVAR(2)
#endif
			stack.pushvar(state->framestack.sp[2]);
			break;
		case OP_PUSHV3:
#ifdef DEBUG
			PUSHLVAR(3)
#endif
			stack.pushvar(state->framestack.sp[3]);
			break;
		case OP_PUSHV4:
#ifdef DEBUG
			PUSHLVAR(4)
#endif
			stack.pushvar(state->framestack.sp[4]);
			break;
		case OP_PUSHV5:
#ifdef DEBUG
			PUSHLVAR(5)
#endif
			stack.pushvar(state->framestack.sp[5]);
			break;
		case OP_PUSHVA	:
			a = stack.pop();
#ifdef DEBUG
			printStr(&fun->vars[AS_WORD(ip)].name);
			sprintf(s,"[%d]", a->as.i);
			printf("%-15s",s);
#endif
			stack.pushvar(state->framestack.sp[AS_WORD(ip)].as.array->vars[a->as.i]);
			ip += WORD_SIZE;
			break;
		case OP_INCI: state->framestack.sp[AS_WORD(ip)].as.i++; ip += WORD_SIZE;break;
		case OP_INCF: state->framestack.sp[AS_WORD(ip)].as.f++; ip += WORD_SIZE;break;
		case OP_DECI: state->framestack.sp[AS_WORD(ip)].as.i--; ip += WORD_SIZE; break;
		case OP_DECF: state->framestack.sp[AS_WORD(ip)].as.f--; ip += WORD_SIZE; break;
		case OP_STOREVA:storeA(state, fun, AS_WORD(ip)); ip += WORD_SIZE;break;
		case OP_STOREV1:store(state, fun, 1, true); break;
		case OP_STOREV0:store(state, fun, 0, true); break;
		case OP_STOREV2:store(state, fun, 2, true); break;
		case OP_STOREV3:store(state, fun, 2, true); break;
		case OP_STOREV4:store(state, fun, 2, true); break;
		case OP_STOREV :store(state, fun, AS_WORD(ip), true); ip += WORD_SIZE; break;
		case OP_PLUS:
			POP_OPERANDS_THEN
			//STR_CONCAT_THEN
			MATH_OP(a, +, b)
			break;
		case OP_POWER:
			POP_OPERANDS_THEN
			stack.pushf(doPower(b->as.f, a->as.f));
			break;
		case OP_MINUS:	POP_OPERANDS_THEN MATH_OP(a, -, b)	break;
		case OP_MULT:	POP_OPERANDS_THEN MATH_OP(a, *, b)	break;
		case OP_DIV:	POP_OPERANDS_THEN 
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
		case OP_GT:	POP_OPERANDS_THEN DO_LOGIC_OP(a, > , b)		break;
		case OP_GE:	POP_OPERANDS_THEN DO_LOGIC_OP(a, >= , b)	break;
		case OP_LT:	POP_OPERANDS_THEN DO_LOGIC_OP(a, < , b)		break;
		case OP_LE:	POP_OPERANDS_THEN DO_LOGIC_OP(a, <= , b)	break;
		case OP_AND:POP_OPERANDS_THEN DO_LOGIC_OP(a, &&, b)		break;
		case OP_OR:	POP_OPERANDS_THEN DO_LOGIC_OP(a, || , b)	break;
		case OP_EQ:
			POP_OPERANDS_THEN
			if (a->type == V_STRING && b->type == V_STRING) 
				stack.pushb( strcmp(state->constants.at(a->as.i).as.s,
					state->constants.at(b->as.i).as.s)==0);
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
			funIndex = READ_WORD(ip);	// function index
			count	 = READ_WORD(ip);	// Count of parameter
#ifdef DEBUG
			printStr(&state->functions[funIndex]->des.name);
#endif
			//call normal function
			if (state->functions[funIndex]->des.type == V_FUNCT)
			{
				TFunction* pFun = state->functions[funIndex];
				_run(state, pFun);
				//restore sp
				if (state->framestack.stack.size()) {
					state->framestack.sp = &state->framestack.stack.at(prevSStackSize);
				}
			}//call a C function
			else if (state->functions[funIndex]->des.type == V_CFUNCT) {
				state->functions[funIndex]->pfun.cfun(state, &stack, count);
#ifdef DEBUG
				printf("%-6s"," ");
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
		SHOWSTACK
#endif
	}
	printf("%p: %-10s ", ip, OPCODESTR((OPCODE)*ip));
  }

//5030 broadway suite 707
void CVM::run(CState *state)
{
	_run(state, state->global);
	if (state->getError()>ERRORS)
	{
		printf("Runtime error :%s\n", GETERROR(state->getError()));
	}
	state->framestack.stack.clear();
}