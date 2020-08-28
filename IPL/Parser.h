#pragma once
#include "Tokenizer.h"
#include "vm.h"
#include "CodGen.h"

#define writeCode(source) source->pfun.bytecode

struct TScoupVarInfo {
	TDes des;
	int index;
};

struct TMeta
{
	TDes self;
	std::vector<TDes> children;
	int find() 
	{
	
	}
};

//helping struct that permit to have breaks or continues
struct TGroup
{
	TFunction *parent;
public:
	TGroup(TFunction* parent) {
		this->parent = parent;
	}
	std::vector<TScoupVarInfo> vars;
	std::vector<short> continues;	//labels to all continues in a function
	std::vector<short> breaks;		//labels to all returns of a function
	std::vector<short> returns;	//labels to all returns of a function
	int findVar(const TString &varName, bool &outLocal);
	int addVar(const TString& varName, VarType type, int index);
};

class CParser
{
	CState *state;
	Tokenizer tokenizer;
	int begins, ifs, whiles, fors;
public:
	CParser(CState *state);
	~CParser();
	void parseClassDef				(TGroup& group);
	void parseFunctionCall			(TGroup& group, Token &FunName);
	unsigned parseAssignment		(TGroup& group, unsigned token);
	int createVarIfnotExist			(TGroup& group, TString& varName, VarType v, bool& outLocal);
	void praseExpresionOrFuncCall	(TGroup& group);
	void functionDeclaration		(TGroup& group);
	bool emitCommoNumber			(TFunction* fun, int number);
	void emitInteger				(TFunction* fun, int number);
	int addConstant					(Token& token);
	void emitFloat					(TFunction* fun,float number);
	int parseParamList				(TGroup& group);
	int storeParamlist				(TGroup& group);
	void parseFactor				(TGroup& group);
	void parseTerm					(TGroup& group);
	void parseExpresion				(TGroup& group);
	void parseIF					(TGroup& group);
	void parseForLoop				(TGroup& group);
	void parseReturn				(TGroup& group);
	void parseContinue				(TGroup& );
	void parseBreak					(TGroup& group);
	bool parseStatement				(TGroup&);
	void parseBlock					(TGroup& group);
	bool parse(const char *s);
	void expected(const TString*, const char *whatExpected);
	void error(const TString* what, const char* why);
	void error(const char *what, const char* why);
	int match(const TTOKEN chr, const char*error);
};