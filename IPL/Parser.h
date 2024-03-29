#pragma once
#include "Tokenizer.h"
#include "vm.h"
#include "CodGen.h"

#define writeCode(source) source->bytecode

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
class TGroup
{
	friend class CParser;
	std::vector<TScoupVarInfo> vars;
	std::vector<short> continues;	//labels to all continues in a function
	std::vector<short> breaks;		//labels to all returns of a function
	//std::vector<short> returns;	//labels to all returns of a function
	TObjectDef *fun;
	TGroup* parent;
public:
	TGroup(TGroup* Parent, TObjectDef* fun) {
		this->fun = fun;
		this->parent = Parent;
	}
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
	void parseAssignment			(TGroup& group, unsigned token);
	int createVarIfnotExist			(TGroup& group, TString& varName, VarType v, bool& outLocal);
	void parseExpresionOrFuncCall	(TGroup& group);
	void parseFunctionDeclaration	(TGroup& group);
	unsigned parseFunctionDec(TGroup& group);
	int addConstant					(Token& token);
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