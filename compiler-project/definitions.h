
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

enum single_tokens { APOST = '\'', PERIOD = '.', COLON = ':', SEMICOLON = ';', LPAREN = '(', RPAREN = ')', DIVIDE = '/', TIMES = '*', PLUS = '+', MINUS = '-', LBRACK = '[', RBRACK = ']', AMP = '&', GREATER = '>', LESS = '<' };
enum multi_tokens { INTEGER = 257, BOOL, FLOAT, CHAR, STRING, R_INTEGER, R_FLOAT, R_BOOL, R_CHAR, 
					R_STRING, GLOBAL, COMMENT, WHILE, FOR, IF, THEN, ELSE, BEGIN, F_END, END, 
					TRUE, FALSE, NOT, GE, LE, ASSIGN, EQUAL, NE, PROGRAM, IS, IN, OUT, INOUT, 
					PROCEDURE, RETURN, IDENTIFIER, PROC_TYPE, BUILTIN, UNKNOWN };

//  GETFLOAT, GETINTEGER, GETSTRING, GETCHAR, GETBOOL, PUTFLOAT, PUTINTEGER, PUTSTRING, PUTCHAR, PUTBOOL,

struct token_t
{
	int type;
	int lineNumb;
	union
	{
		char stringValue[256];
		int intValue;
		double doubleValue;
	} val;
};

struct Parameter
{
	int type;
	int mode;	
};

struct Value
{
	int type;
	std::string name;
	bool isIdentifier;
    bool isGlobal;
	std::vector<Parameter> paramList;
	int reg;
	int addrReg;
	union
	{
		char stringValue[256];
		int intValue;
		double doubleValue;
	} val;
};

inline std::ostream& operator<<(std::ostream& os, const multi_tokens& token)
{
	switch (token)
	{
	case INTEGER: os << "INTEGER"; break;
	case FLOAT: os << "FLOAT"; break;
	case CHAR: os << "CHAR"; break;
	case STRING: os << "RESERVED STRING"; break;
	case R_INTEGER: os << "RESERVED INTEGER"; break;
	case R_FLOAT: os << "RESERVED FLOAT"; break;
	case R_BOOL: os << "RESERVED BOOL"; break;
	case R_CHAR: os << "RESERVED CHAR"; break;
	case R_STRING: os << "STRING"; break;
	case GLOBAL: os << "GLOBAL"; break;
	case WHILE: os << "WHILE"; break;
	case FOR: os << "FOR"; break;
	case IF: os << "IF"; break;
	case THEN: os << "THEN"; break;
	case ELSE: os << "ELSE"; break;
	case BEGIN: os << "BEGIN"; break;
	case END: os << "END"; break;
	case TRUE: os << "TRUE"; break;
	case FALSE: os << "FALSE"; break;
	case NOT: os << "NOT"; break;
	case GE: os << "GE"; break;
	case LE: os << "LE"; break;
	case ASSIGN: os << ":="; break;
	case EQUAL: os << "EQUAL"; break;
	case NE: os << "NE"; break;
	case PROGRAM: os << "PROGRAM"; break;
	case IS: os << "IS"; break;
	case IN: os << "IN"; break;
	case OUT: os << "OUT"; break;
	case PROCEDURE: os << "PROCEDURE"; break;
	case RETURN: os << "RETURN"; break;
	case IDENTIFIER: os << "IDENTIFIER"; break;
	case UNKNOWN: os << "UNKNOWN"; break;
	case COMMENT: os << "COMMENT"; break;
	default:;
	}

	return os;
}



#endif
