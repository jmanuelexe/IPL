#include "Tokenizer.h"
#include <string.h>

Reserved row2[] = {		//reserved words with lenth of two characters
	{ "if", TK_IF },
	{ "do", TK_DO },
	{ "or", TK_OR },
	{ "to", TK_TO },
	0
};
Reserved row3[] = {	//reserved words with lenth of three characters
	{ "fun", TK_FUNCTION },
	{ "for", TK_FOR },
	{ "end", TK_END },
	{ "not", TK_NOT },
	{ "and", TK_AND },
	0
};
Reserved row4[] = {
	{ "then", TK_THEN },
	{ "true", TK_TRUE },
	{ "case", TK_CASE },
	{ "else", TK_ELSE },
	{ "goto", TK_GOTO },
	0
};
Reserved row5[] = {
	{ "false", TK_FALSE },
	{ "while", TK_WHILE },
	{ "break", TK_BREAK },
	{ "label", TK_LABEL },
	{ "range", TK_RANGE },
	{ "class", TK_CLASS },
	{ "begin", TK_BEGIN },
	{ "endif", TK_ENDIF },
	0
};
Reserved row6[] = {
	{ "switch", TK_SWITCH },
	{ "return", TK_RETURN },
	{ "endfor", TK_ENDFOR },	
	0
};
Reserved row8[] = {
	{ "continue", TK_CONTINUE },
	{ "endwhile", TK_ENDWHILE },
	{ 0 }
};

Reserved *Keyboards[] = { NULL, NULL, row2, row3,row4, row5, row6, NULL, row8 };//makesure the last keyboard is zero as string

void Tokenizer::init()
{
	int n;

	for (n = 0; n < 64; n++) chcomp[n] = ttError;
	for (n = 'A'; n <= 'Z'; n++) chcomp[n] = ttLeter;
	for (n = 'a'; n <= 'z'; n++) chcomp[n] = ttLeter;
	for (n = '0'; n <= '9'; n++) chcomp[n] = ttDigit;
	chcomp['_'] = ttLeter;

	chcomp['\n'] = chcomp['\t']= chcomp[' '] = ttWhitespace;

	chcomp['.'] = chcomp['{'] = chcomp['}'] =
	chcomp[')'] = chcomp['('] = chcomp[','] = chcomp['='] = 
	chcomp['>'] = chcomp['<'] = chcomp['\'']= chcomp['"'] = 
	chcomp['^'] = chcomp[';'] = chcomp['+'] = chcomp['-'] = 
	chcomp['['] = chcomp[']'] = chcomp['-'] = chcomp['!'] =
	chcomp['*'] = chcomp['/'] = ttSpecial;
}

Tokenizer::Tokenizer()
{
	lasttoken.text.begin = curToken.text.begin = nextToken.text.begin = 0;
	lasttoken.type = curToken.type = nextToken.type = TK_NULL;
	lasttoken.col=curToken.col = nextToken.col = TK_NULL;
	s = 0;
	col = line = 1;
	init();
}

TTOKEN Tokenizer::isKeyWord(const Token& token) const
{
	const char* s1, *s2;
	Reserved *prw;

	if (token.text.length >= 2 && token.text.length <= 8)
	{
		prw = Keyboards[token.text.length];
		if (prw) {
			for (; prw->name; ++prw)
			{
				s1 = prw->name;
				s2 = token.text.begin;
				while (*s1 == *s2 && s2 < token.text.end) 
				{
					s1++; s2++;
				}
				if (s2 == token.text.end)
					return prw->type;
			}
		}
	}
	return TK_IDENTIFIER;
}

void Tokenizer::getIdentifier()
{	
	while (*s != NULL) {
		*s++;
		if (chcomp[*s] < ttLeter)
			break;
	}

	nextToken.text.end=s;
	nextToken.text.length = (int)(nextToken.text.end - nextToken.text.begin);
	nextToken.type = isKeyWord(nextToken);
}

void Tokenizer::getNumber()
{
	char decilmalpoint = 0;

	while (s != NULL) {
		*s++;
		if (*s == '.') decilmalpoint++;
		if (chcomp[*s] != ttDigit && *s != '.')	break;
	}

	nextToken.text.end = s;
	nextToken.text.length = (int)(nextToken.text.end - nextToken.text.begin);
	if (decilmalpoint) nextToken.type = TK_FLOAT;
	else nextToken.type = TK_INTEGER;
}

void Tokenizer::parseString(char delimiter)
{ 
	while (*s++ != delimiter &&*s!='\0') *s++;

	nextToken.text.end = s;
	nextToken.text.length = (int)(nextToken.text.end - nextToken.text.begin);
}

//skip until finds a string specified as delimiter
void Tokenizer::skipUntil(const char *delimiter)
{
	const char *d = delimiter;
	
	do {
		nextChar();
		if (*d == *s)
			d++;
		else if (d != delimiter) d = delimiter;

	} while (*d!='\0');
	nextChar();
}

char Tokenizer::nextChar()
{
	switch (*s++)
	{
	case TK_NEWLINE: line++; col = 1; 	break;
	default:
		col++;
	}
	return *s;
}

void Tokenizer::newLine() 
{
	nextToken.col = col;
	nextToken.line = line;
	line++; col = 0;
}

void Tokenizer::parseSpecial()
{
	char ch;
	
	ch = *s++;
	
	switch (ch)
	{	
	case '"' :	//long string enclosed by either " or ' are valid
	case '\'':
		nextToken.text.begin = s;	//begin must point to s after the " or '
		while (*s && ch != *s)
		{
			nextChar();
		}

		if (*s == ch) {
			nextToken.type = TK_STRING;
			nextToken.text.end = s;
			nextToken.text.length = int(nextToken.text.end - nextToken.text.begin);
			s++;
		}
		else { 
			nextToken.type = TK_ERRORPARSE; 
		}
		return;
	case '/' :
		if (*s == '*') 
		{				//long comment
			skipUntil("*/");
			break;
		}else if (*s == '/') 
		{
			skipUntil("\n");			//short comment
			break;
		} else 	nextToken.type = TK_DIV;
		break;	
	case '(': nextToken.type = TK_OPENPARENTHESIS; break;
	case ')': nextToken.type = TK_CLOSEPARENTHESIS;break;
	case '=': 
		if (*s == '=') 
			nextToken.type = TK_EQEQ;
		else nextToken.type = TK_EQ;
		break;
	case '!':
		if (*s == '=') 
		{
			nextToken.type = TK_DIF;
			nextChar();
		}
		else nextToken.type = TK_NOT;
		break;
	case '>':
		if (*s == '=') 
		{
			nextToken.type = TK_GE;
			nextChar();
		}
		else nextToken.type = TK_GT;
		break;
	case '<':
		if (*s == '=') 
		{
			nextToken.type = TK_LE;
			nextChar();
		}
		else nextToken.type = TK_LT;
		
		break;
	case '-':
		if (*s == '=') 
		{
			nextToken.type = TK_DEC;
			nextChar();
		}
		else if (*s == '-')
		{
			nextToken.type = TK_LESSLESS;
			nextChar();
		}
		else nextToken.type = TK_MINUS;
		break;
	case '+':
		if (*s == '=') 
		{
			nextToken.type = TK_INC;
			nextChar();
		}
		else if (*s == '+')
		{
			nextToken.type = TK_PLUSPLUS;
			nextChar();
		}
		else nextToken.type = TK_PLUS;
		break;
	case '*': nextToken.type = TK_MULT; break;
	case '//':nextToken.type = TK_DIV; break;
	case '^': nextToken.type = TK_POWER; break;
	case ',': nextToken.type = TK_COMMA; break;
	case ';': nextToken.type = TK_SEMICOLON; break;
	case '{': nextToken.type = TK_OPENCURLYBRACKET; break;
	case '}': nextToken.type = TK_CLOSECURLYBRACKET; break;
	default:
		nextToken.type = (TTOKEN) ch;
		break;
	}

	nextToken.text.end = s;
	nextToken.text.length = int(nextToken.text.end - nextToken.text.begin);
}
/*
Token Tokenizer::nextToken() 
{
	const char* p = s;
	Token ltoken = lasttoken;
	Token t = token;
	Token r;

	int c	= col;
	int l	= line;
	nextToken();
	r = token;
	col		= c;
	line	= l;
	token	= t;
	s = p;
	return r;
}*/

void Tokenizer::setSource(const char* source)
{
	s = source;
	next();
}

void Tokenizer::scan() 
{
	lasttoken = curToken;
	curToken = nextToken;
	next();
}

void Tokenizer::next()
{
	const char* cur; //if first char could be an ttWhitespace so cur mayno be initialize so do it now

	again:
		cur = s;
		if (!s || !*s) {
			nextToken.type = TK_EOF; return;
		}

		nextToken.text.length = 0;
		nextToken.col = col;
		nextToken.line = line;
		nextToken.text.begin = s;

		switch (chcomp[*s])
		{
		case ttDigit: getNumber();		break;
		case ttLeter: getIdentifier();  break;
		case ttSpecial: parseSpecial(); break;
		case ttWhitespace:
		default:nextChar(); goto again;
		}
	
	col +=int( s-cur);
}