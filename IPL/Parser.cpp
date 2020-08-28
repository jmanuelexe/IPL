#include "Parser.h"
#include <stdlib.h>
#include <stdio.h>
#pragma warning(disable : 4996)
OPCODE opshortcut[2][2] = { { OP_INCF,OP_INCI},
									{ OP_DECF,OP_DECI} };

int TGroup::findVar(const TString& varName, bool& outLocal)
{
	for (int index = 0; index < (int)vars.size(); index++) 
	{
		if(strEq(&vars[index].des.name, &varName))
			return index;	
	}

	return NOFOUND;
}

int TGroup::addVar(const TString& varName, VarType type, int index)
{
	TScoupVarInfo v;
	v.des.name = varName;
	v.des.type = type;
	v.index = index;
	vars.emplace_back(v);
	return (int)vars.size()-1;
}

/**********************************************************************************
*
*
**********************************************************************************/
CParser::~CParser()
{
}

CParser::CParser(CState *State)
{
	this->state = State;
	begins = ifs = whiles = fors = 0;
}

void CParser::parseFunctionDeclaration(TGroup& group)
{
	tokenizer.scan();
	tokenizer.curToken.type = TK_FUNCTION;
	
	int fIndex = group.parent->findFunction(state, &tokenizer.curToken.text);

	if (fIndex != NOFOUND) {
		error(&tokenizer.curToken.text, "Function already defined");
		return;
	}
	
	fIndex = state->createSymbol(group.parent, tokenizer.curToken.text, V_FUNCT, 0);
	
	if (fIndex == NOFOUND) {
		error(&tokenizer.curToken.text, "Unable to create.");
		return;
	}

	Function* fun = state->funReg[fIndex];

	tokenizer.scan();
	
	TGroup thisFun(fun);
	storeParamlist(thisFun);
	begins++;
	match(TK_OPENCURLYBRACKET, "{");

	parseBlock(thisFun);
	match(TK_CLOSECURLYBRACKET, "}");

	begins--;

	//update returns to jump to the end of the function
	for (unsigned n = 0; n < fun->returns.size(); n++)
	{	
		writeCode(fun)->endJump(fun->returns[n]);
	}
	
	//restore last scope
	if (!state->isError()) writeCode(fun)->emit(OP_EXIT);
}

void CParser::parseBreak(TGroup& group)
{
	tokenizer.scan();
	if (!state->isError()) 
	{
		group.breaks.push_back((short)writeCode(group.parent)->beginjump(OP_JMP));
	}
}

int CParser::parseParamList(TGroup& group)
{
	int param = 0;
	match(TK_OPENPARENTHESIS, "(");
	
	if (tokenizer.curToken.type != TK_CLOSEPARENTHESIS)
	{
		do {
			parseExpresion(group);
			param++;
			if (tokenizer.curToken.type == TK_COMMA) {
				tokenizer.scan();
				continue;
			}
			else break;
		} while (1);
	}
	match(TK_CLOSEPARENTHESIS, ")");
	return param;
}

int CParser::storeParamlist(TGroup& group)
{
	match(TK_OPENPARENTHESIS, "(");
	std::vector<Token> tokens;

	//collect the parameters list
	do {
		if (tokenizer.curToken.type == TK_IDENTIFIER)
			tokens.push_back(tokenizer.curToken);
		else if (tokenizer.curToken.type != TK_COMMA) break;
		tokenizer.scan();
	} while (1);

	match(TK_CLOSEPARENTHESIS, ")");
	
	//Put the parameter in invers order
	for (int i = (int)tokens.size() - 1; i >= 0; i--)
	{
		word index = state->createSymbol(group.parent, tokens[i].text, V_VAR, 0);
		group.addVar(tokens[i].text, V_VAR, index);
		writeCode(group.parent)->emit1i(OP_STOREV, index);
	}
	
	group.parent->param = int(tokens.size());

	return (int)tokens.size();
}

void CParser::parseFunctionCall(TGroup& group, Token &FunName)
{
	FunName.type = TK_FUNCTION;

	int fIndex = group.parent->findFunction(state, &FunName.text);
	//check if it was global
	if (fIndex== NOFOUND) 
	{
		fIndex = state->global->findFunction(state, &FunName.text);
	}

	if (fIndex!=NOFOUND)
	{
		int numparam = parseParamList(group);
		Function*obj = state->funReg[fIndex];
		if (obj->param != numparam && obj->des.type != V_CFUNCT)
		{
			char s[80];
			sprintf(s, "%.*s requires %d parameters but you are passing %d",
				obj->des.name.length, obj->des.name.begin, obj->param, numparam);
			error("Mismatch number of parmeters", s);
		}
		else {
			writeCode(group.parent)->emit2w(OP_CALL, fIndex, numparam);
		}
	}
	else {
		error(&FunName.text, "Function, must be defined before it can be call");

	}
}

//Assigmen or function call
unsigned CParser::parseAssignment(TGroup& group, unsigned index)
{
	tokenizer.scan();
	if (tokenizer.nextToken.type != TK_SEMICOLON) {
		parseExpresion(group);
		writeCode(group.parent)->emit1w(OP_STOREV, index);
	} else {
		switch (tokenizer.curToken.type) {
		case TK_STRING:	writeCode(group.parent)->emit2w(OP_SETS, index, addConstant(tokenizer.curToken));	break;
		case TK_INTEGER:	writeCode(group.parent)->emit2w(OP_SETI, index, (int)strtol(tokenizer.curToken.text.begin, 0, 0)); break;
		case TK_FLOAT:		writeCode(group.parent)->emit1i1f(OP_SETF, index, (NUMBER)strtod(tokenizer.curToken.text.begin, 0)); break;
		case TK_TRUE :		writeCode(group.parent)->emit1i(OP_SETB, index); writeCode(group.parent)->emitB(1); break;
		case TK_FALSE:		writeCode(group.parent)->emit1i(OP_SETB, index); writeCode(group.parent)->emitB(0); break;
		}
		tokenizer.scan();
	}
	
	return index;
}

int CParser::createVarIfnotExist(TGroup& group, TString &varName, VarType type, bool &outLocal)
{	
	int localIndex;
	localIndex = group.findVar(varName, outLocal);

	if (localIndex == NOFOUND) 
	{		
		localIndex = group.addVar(varName, type, state->createSymbol(group.parent, varName, type, 0));
		outLocal = true;
	}

	return localIndex;
}

void CParser::parseExpresionOrFuncCall(TGroup& group)
{
	int row = 0, col = 0;
	Token var_or_func;
	int index;
	bool local;
	OPCODE op;
	var_or_func = tokenizer.curToken;
	tokenizer.scan();

	switch (tokenizer.curToken.type)
	{	
	case TK_ERRORPARSE: error(&tokenizer.curToken.text, "Invalid token"); break;
	case TK_PLUSPLUS:
	case TK_LESSLESS:
		var_or_func.type = TK_IDENTIFIER;		
		index = createVarIfnotExist(group, var_or_func.text, V_VAR, local);
		if (index == NOFOUND) {
			error(&var_or_func.text, "can not be created");
			return;
		}

		if (tokenizer.curToken.type == TK_LESSLESS) row++;
		if (group.parent->vars[index].type == V_INT) col++;
		
		op = opshortcut[row][col];
		writeCode(group.parent)->emit1w(op, index);
		tokenizer.scan();

		break;
	case TK_COMMA:
		index = createVarIfnotExist(group, var_or_func.text, V_VAR, local);
		tokenizer.scan();
		parseExpresionOrFuncCall(group);
		break;
	case '[':
		tokenizer.scan();
		parseExpresion(group);
		match(TTOKEN(']'), "clossing bracket spected");
		printf("\nArray\n");
		match(TK_EQ, "Equal sign not found.");
		parseExpresion(group);

		index = createVarIfnotExist(group, var_or_func.text, V_ARRAY, local);
		
		if (index == NOFOUND) 
			error(&var_or_func.text, "can't be created");
		else{
			writeCode(group.parent)->emit1w(OP_STOREVA, index);
		}

		break;
	case TK_EQ:	
		index = createVarIfnotExist(group, var_or_func.text, V_VAR, local);
		if(index == NOFOUND) error(&var_or_func.text, "can't be created");
		else
			parseAssignment(group, index);

		break;
	case TK_OPENPARENTHESIS:
		parseFunctionCall(group, var_or_func);
		break;
	default:
		if (var_or_func.type == TK_IDENTIFIER)
		{
			index = createVarIfnotExist(group, var_or_func.text, V_VAR, local);
			if (index == NOFOUND) error(&var_or_func.text, "Identifier not found");
			//writeCode(state).emit1i(OP_PUSHVA, index);
			//writeCode(state).emit1i(OP_PUSHV, index);
		}
		else
		{
			error("unknown", "token found!");
		}
		break;
	}
}

bool CParser::emitCommoNumber(Function* fun, int number)
{
	if (number == 0)	  writeCode(fun)->emit(OP_PUSHC0);
	else if (number == 1) writeCode(fun)->emit(OP_PUSHC1);
	else if (number == 2) writeCode(fun)->emit(OP_PUSHC2);
	else if (number == 3) writeCode(fun)->emit(OP_PUSHC3);
	else return false;
	return true;
}

int CParser::addConstant( Token& token) 
{
	TVariant v;
	//VarType
	switch (token.type) {
	case TK_FLOAT:
		v.as.f = (NUMBER)strtof(token.text.begin, 0);
		v.type = V_FLOAT;
		break;
	case TK_INTEGER:
		v.as.i = strtol(token.text.begin, 0, 0);
		v.type = V_INT;
		break;
	case TK_STRING:
		v.as.s = new char[token.text.length + 1];
		memcpy((v.as.s+1), token.text.begin, (size_t)token.text.length);
		v.type = V_STRING;
		v.as.s[0] = token.text.length;
		break;
	}

	state->constReg.push_back(v);
	return (int)state->constReg.size() - 1;
}

void CParser::parseFactor(TGroup& group)
{
	Token varName;
	int index;
	bool local;
	
	switch (tokenizer.curToken.type)
	{
	case TK_OPENPARENTHESIS:
		match(TK_OPENPARENTHESIS, "(");
		parseExpresion(group);
		match(TK_CLOSEPARENTHESIS, ")");
		return;
	case TK_MINUS:
		match(TK_MINUS, "-");
		parseExpresion(group);
		return;
	case TK_PLUS:
		match(TK_PLUS, "+");
		parseExpresion(group);
		return;
	case TK_FLOAT:	
		writeCode(group.parent)->emit1f(OP_PUSHF, (float)strtod(tokenizer.curToken.text.begin, NULL));/*addConstant(tokenizer.curToken));*/ break;
	case TK_INTEGER:
		writeCode(group.parent)->emit1i(OP_PUSHI, (int)strtol(tokenizer.curToken.text.begin, NULL, 0)/*addConstant(tokenizer.curToken)*/);	
		break;
	case TK_TRUE : writeCode(group.parent)->emitBool(OP_PUSHB, true);	break;
	case TK_FALSE: writeCode(group.parent)->emitBool(OP_PUSHB, false);	break;
	case TK_STRING:
		writeCode(group.parent)->emit1w(OP_PUSHS, addConstant(tokenizer.curToken));	break;
		break;
	case TK_IDENTIFIER:
		varName = tokenizer.curToken;//save the var name
		tokenizer.scan();

		if (tokenizer.curToken.type == TK_OPENPARENTHESIS) 
		{
			parseFunctionCall(group, varName);
		}
		else if(tokenizer.curToken.type == TTOKEN('['))
		{
			tokenizer.scan();
			parseExpresion(group);
			match(TTOKEN(']'), "clossing bracket spected");

			index = createVarIfnotExist(group, varName.text, V_VAR, local);
			writeCode(group.parent)->emit1w(OP_PUSHVA, index);
		}
		else 
		{
			index = createVarIfnotExist(group, varName.text, V_VAR, local);
			if (index == NOFOUND)
				error(&varName.text, "can't be created");

			writeCode(group.parent)->emit1i(OP_PUSHV, index);
		}
		return;
	default:	expected(&tokenizer.curToken.text, "Number or variable");
	}

	tokenizer.scan();
	
	return;
}

int CParser::match(const TTOKEN chr, const char*error)
{
	//if (state->getError()) return 0;
	if (tokenizer.curToken.type != chr )
	{
		//if fail show result
		expected(&tokenizer.curToken.text, error);
		
		//try again
		tokenizer.scan();
		if (tokenizer.curToken.type != chr)
		return 0;
	}
	tokenizer.scan();
	return 1;
}

void CParser::expected(const TString* butfound, const char *expectedwhat)
{
	printf("Error at(%d, %d): Expected '%s' but found '", tokenizer.line, tokenizer.curToken.col, expectedwhat);
	printStr(butfound);
	printf("' after '");
	printStr(&tokenizer.lasttoken.text);
	printf("'.\n");
	state->setError(1);
}

void CParser::error(const TString* what, const char* why) 
{
	printf("Error: '%.*s'. %s.\n", what->length, what->begin, why);
	state->setError(1);
}

void CParser::error(const char * what, const char* why)
{
	printf("Error: '%s'. %s.\n", what, why);
	state->setError(1);
}

void CParser::parseTerm(TGroup& group)
{
	TTOKEN type;
	parseFactor(group);
	while (tokenizer.curToken.type == TK_MULT
		|| tokenizer.curToken.type == TK_DIV
		|| tokenizer.curToken.type == TK_GT
		|| tokenizer.curToken.type == TK_GE
		|| tokenizer.curToken.type == TK_LT
		|| tokenizer.curToken.type == TK_LE
		|| tokenizer.curToken.type == TK_EQ
		|| tokenizer.curToken.type == TK_AND
		|| tokenizer.curToken.type == TK_POWER){
		type = tokenizer.curToken.type;
		tokenizer.scan();
		parseFactor(group);

		switch (type){
		case TK_MULT:	writeCode(group.parent)->emit(OP_MULT);		break;
		case TK_DIV:	writeCode(group.parent)->emit(OP_DIV);		break;
		case TK_AND:	writeCode(group.parent)->emit(OP_AND);		break;
		case TK_GT:		writeCode(group.parent)->emit(OP_GT);		break;
		case TK_GE:		writeCode(group.parent)->emit(OP_GE);		break;
		case TK_LT:		writeCode(group.parent)->emit(OP_LT);		break;
		case TK_LE:		writeCode(group.parent)->emit(OP_LE);		break;
		case TK_EQ:		writeCode(group.parent)->emit(OP_EQ);		break;
		case TK_POWER:	writeCode(group.parent)->emit(OP_POWER);	break;
		}
	}
	return;
}

void CParser::parseExpresion(TGroup& group)
{
	TTOKEN type;
	parseTerm(group);
	while (tokenizer.curToken.type == TK_PLUS
		|| tokenizer.curToken.type == TK_MINUS
		|| tokenizer.curToken.type == TK_OR)
	{
		type = tokenizer.curToken.type;
		tokenizer.scan();
		parseTerm(group);

		switch (type)
		{
		case TK_PLUS:	writeCode(group.parent)->emit(OP_PLUS);		break;
		case TK_MINUS:	writeCode(group.parent)->emit(OP_MINUS); 	break;
		case TK_OR:		writeCode(group.parent)->emit(OP_OR);		break;
		}
	}	
	return;
}

void CParser::parseIF(TGroup& group)
{
	word l1 = 0, l2 = 0;
	
	ifs++;	//entering an if stamentes
	tokenizer.scan();
	parseExpresion(group);

	if(match(TK_THEN, "Then"))
		l1 = writeCode(group.parent)->beginjump(OP_JMZ);//jump if false to l1
	
	if (tokenizer.curToken.type == TK_SEMICOLON && 
		tokenizer.curToken.text.begin[0] == 0)
		tokenizer.scan();

	//if true statement
	TGroup innerGroup(group.parent);
	parseBlock(innerGroup);
	
	if(tokenizer.curToken.type == TK_ELSE) 
	{
		tokenizer.scan();
		l2 = writeCode(group.parent)->beginjump(OP_JMP); //jump regardless
		writeCode(group.parent)->endJump(l1);
		TGroup innerGroup2(group.parent);
		parseBlock(innerGroup2);
	}

	if(match(TK_ENDIF, "EndIf") )
	{
		ifs--;	//leaving an if stamentes
		if(!l2)	//no else found so update label1(jump if false)
			writeCode(group.parent)->endJump(l1);
		else 
			writeCode(group.parent)->endJump(l2);
	}
}

void CParser::parseForLoop(TGroup& group)
{
	short l2 = 0;
	word varIndex;
	bool local;
	tokenizer.scan();

	Token var = tokenizer.curToken;

	fors++;
	if (!match(TK_IDENTIFIER, "Identifier")) return;
	varIndex = createVarIfnotExist(group, var.text, V_VAR, local);
	
	parseAssignment(group, varIndex);

	if (!match(TK_TO, "To")) return;
	l2 = writeCode(group.parent)->getCurPos();
	parseExpresion(group);			

	TMeta parent;
	TGroup forGroup(group.parent);
	//loop statements
	parseBlock(forGroup);

	for (unsigned n = 0; n < forGroup.continues.size(); n++)
	{
		writeCode(group.parent)->endJump(forGroup.continues[n]);
	}

	writeCode(group.parent)->emit1i(OP_PUSHV, varIndex);
	writeCode(group.parent)->emit(OP_EQ);
	writeCode(group.parent)->emit1w(OP_INCI, varIndex);
	int jumptto = (int)(/*(writeCode(group.parent)->getCurPos() /*- */l2/*+1)*/);
	writeCode(group.parent)->emit1w(OP_JMZ, jumptto);
	if(match(TK_ENDFOR, "EndFor")) fors--;

	for (unsigned n = 0; n < forGroup.breaks.size(); n++)
	{
		writeCode(group.parent)->endJump(forGroup.breaks[n]);
	}
}

void CParser::parseContinue(TGroup& group)
{
	tokenizer.scan();
	if (!state->isError()) {
		group.continues.push_back((short)writeCode(group.parent)->beginjump(OP_JMP));
	}
}

void CParser::parseReturn(TGroup& group)
{
	tokenizer.scan();
	if (tokenizer.curToken.type != TK_SEMICOLON) {
		parseExpresion(group);
	}

	if (!state->isError()) {
		if(group.parent->des.type == V_FUNCT)
		   group.parent->returns.push_back((short)writeCode(group.parent)->beginjump(OP_JMP));
	}
}

void CParser::parseClassDef(TGroup& group)
{
	tokenizer.scan();

	if (tokenizer.curToken.type == TK_IDENTIFIER)
	{
		/*Token classname = tokenizer.curToken;
		TParent* parent = NULL;
		parent = group->parent;
		int index = state->createSymbol(parent, &classname.text, V_CLASS, NULL);
		TGroup innerGroup(state->objects[index]);
		begins++;
		tokenizer.scan();
		parseBlock(&innerGroup);
		match(tokenizer.curToken.type, "end");*/
	}
}

bool CParser::parseStatement(TGroup& group)
{	
	switch (tokenizer.curToken.type)
	{
	case TK_IF				: parseIF(group);			break;
	case TK_IDENTIFIER		: parseExpresionOrFuncCall(group);break;
	case TK_OPENPARENTHESIS :
	case TK_STRING			:
	case TK_FLOAT			:
	case TK_INTEGER			: parseExpresion(group);			break;	
	case TK_FOR				: parseForLoop(group); 	break;
	case TK_BREAK			: parseBreak(group);		break;
	case TK_CONTINUE		: parseContinue(group);	break;
	case TK_RETURN			: parseReturn(group);		break;
	case TK_FUNCTION		: parseFunctionDeclaration(group);	return true;
	case TK_CLASS			: parseClassDef(group);break;
		/*
		case TK_WHILE: return parseWhile();
		case TK_SWITCH: return parseSwitch();
		case TK_CASE: return parseCase();
		*/		
	}
	
	if (tokenizer.curToken.type != TK_SEMICOLON && 
		tokenizer.curToken.type != TK_CLOSEPARENTHESIS
		) {
		//if fail show result
		expected(&tokenizer.curToken.text, ";");
		//try again
		tokenizer.scan();
		if (tokenizer.curToken.type != TK_SEMICOLON)
			return 0;
	}
	tokenizer.scan();
//match(TK_SEMICOLON, ";");
	return false;
}

void CParser::parseBlock(TGroup& group)
{
	while (tokenizer.curToken.type != TK_EOF)
	{
		switch (tokenizer.curToken.type) {
		case TK_CLOSECURLYBRACKET: if (!begins)	error("}", "Statement"); return;
		case TK_END		: if (!begins)	error("End", "Statement"); return;
		case TK_ELSE	: if (!ifs)		error("Else", "Statement"); return;
		case TK_ENDIF	: if (!ifs)		error("EndIf", "Statement"); return;
		case TK_ENDFOR	: if (!fors)	error("EndFor", "Statement"); return;			
		default: parseStatement(group); break;
		}
	}
}

struct classe {
	VarList vars;
};

TString makeString(const char *strz) 
{
	TString str;

	str.length = (int)strlen(strz);
	str.begin = strz;
	str.end = str.begin + str.length;
	return str;
}

bool CParser::parse(const char* s)
{
	if(!state) return false;
	
	tokenizer.setSource(s);
	tokenizer.scan();

	TGroup global(state->global);

	parseBlock(global);
	
	if (!state->isError())
	{
		writeCode(state->global)->emit(OP_HALT);
		return true;
	}

	return false;
}