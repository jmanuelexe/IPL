#ifndef PARSER_H
#define PARSER_H
#include "types.h"
#include "State.h"

typedef enum TTOKEN :char
{
	//general tokens
	TK_HALT = -1, TK_NEWLINE = '\n', TK_IDENTIFIER, TK_EOF,
	//data types
	TK_NULL, TK_INTEGER, TK_FLOAT, TK_STRING, TK_TRUE, TK_FALSE,
	//loop
	TK_END, TK_ENDWHILE, TK_ENDFOR, TK_ENDIF, TK_FUNCTIONCAL, TK_PLUSPLUS, TK_LESSLESS, TK_COMMA,
	//logic operators
	TK_NOT, TK_AND, TK_OR,
	//arimethic operators
	TK_ASSIGN, TK_PLUS, TK_MINUS, TK_MULT, TK_DIV, TK_POWER, TK_PERIOD,
	//compare operator
	TK_EQEQ, TK_INC, TK_DEC, TK_LT, TK_GT, TK_EQ, TK_DIF, TK_LE, TK_GE,
	//grouping operator
	TK_OPENPARENTHESIS, TK_CLOSEPARENTHESIS, TK_OPENCURLYBRACKET, TK_CLOSECURLYBRACKET,
	TK_COLON, TK_SEMICOLON,
	//special keyboard
	TK_LABEL, TK_GOTO, TK_FUNCTION,
	TK_RETURN, TK_IF, TK_THEN, TK_ELSE,
	TK_BEGIN, TK_UNTIL,
	TK_FOR, TK_TO, TK_DO, TK_WHILE, TK_CONTINUE, TK_BREAK,
	TK_SWITCH, TK_CASE, TK_RANGE, TK_ROOT,
	TK_CLASS, TK_CONCAT, TK_ERRORPARSE
} TTOKEN;

//character type. The possible classification for the characters found on the source string
typedef enum CTYPE : unsigned char{
	ttError,
	ttWhitespace,
	ttSpecial,
	ttLeter,
	ttDigit
}CTYPE;

//Usefull way to store all the reserved words of the language
struct Reserved {
	const char* name;
	TTOKEN type;
};

//store a token. The token text and type
struct Token
{
	TString text;
	TTOKEN type;					//the type of token it is
	int col, line;
};

class Tokenizer
{
	CTYPE chcomp[127];
	const char* s;		//pointer to the zero terminate string that contains the code
public:
	Token	curToken,
			nextToken,	//current token read
			lasttoken;	//previews token read
	int		col, line;	//column and line number
public:
	void init();
	Tokenizer();
	void setSource(const char *sourse);
	void scan();
private:
	TTOKEN isKeyWord(const Token& toke) const;
	void getNumber();
	void getIdentifier();
	void parseString(const char delimiter);
	void parseSpecial();
	void next();
	void skipUntil(const char *delimiter);
	char nextChar();
	void newLine();
};

#endif
