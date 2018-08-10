#ifndef PARSER_H
#define PARSER_H

#include "definitions.h"
#include "scanner.h"
#include "error.h"
#include "symbol_table.h"
#include <ostream>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>


class Parser
{
private:
	Scanner *scan;
	token_t token;
	ErrorHandler errorHandler;
	bool tokenAccepted;
	SymbolTableManager stManager;
	bool isGlobal;
	std::ofstream assemblyFile;
	int regCount;
	int mainRegCount;
	int labelCount;
	std::vector<Parameter> emptyVector;
	std::vector<std::ostringstream*> procStreamVector;

public:
	Parser(Scanner *scanner)
	{
		scan = scanner;
		errorHandler.SetScanner(scanner);
		stManager.SetErrorHandler(&errorHandler);
		tokenAccepted = true;
		isGlobal = false;
		regCount = 0;
		mainRegCount = 0;
		labelCount = 0;
		
		assemblyFile.open("code.ll", std::ofstream::out);

		if(!assemblyFile.is_open())
		{
			errorHandler.AddError("Failed to open output stream", token, true);
		}
	};
	~Parser(){assemblyFile.close();};

	token_t GetToken()
	{
		if (tokenAccepted)
		{
			token = scan->ScanOneToken();
			tokenAccepted = false;
		}

		return token;
	}

	std::string typeToAssembly(int type)
	{

		if (type == R_INTEGER || type == INTEGER)
		{
			return "i32";
		}
		else if (type == R_FLOAT || type == FLOAT)
		{
			return "double";
		}
		else if (type == R_BOOL || type == BOOL)
		{
			return  "i1";
		}
		else if (type == R_CHAR || type == CHAR)
		{
			return "i8";
		}
		else
		{
			return "";
		}
	}

	std::string typeToStringVal(Value val)
	{
		std::string llvmVal;

		switch(val.type)
		{
			case INTEGER:
				llvmVal = std::to_string(val.val.intValue);
				break;
			case BOOL:
				llvmVal = std::to_string(val.val.intValue);
				break;
			case FLOAT:
				llvmVal = std::to_string(val.val.doubleValue);
				break;
			case CHAR:
				llvmVal = std::to_string(int(val.val.stringValue[0]));
				break;
			default:
				break;
		}

		return llvmVal;
	}

	bool TypeConvert(int expectedType, Value *val)
	{
		switch (expectedType)
		{
		case BOOL:

			if (val->type == BOOL);
			else if (val->type == INTEGER && val->val.intValue != 0)
				val->val.intValue = 1;
			else if (val->type == FLOAT && val->val.doubleValue != 0)
				val->val.intValue = 1;
			else if (val->type == FLOAT && val->val.doubleValue == 0)
				val->val.intValue = 0;
			else if (val->type == CHAR)
				val->val.intValue = 1;
			else
			{
				errorHandler.AddError("Wrong type, expected: " + std::to_string(expectedType), token, true);
				return false;
			}
			break;

		case INTEGER:

			if (val->type == INTEGER);
			else if (val->type == FLOAT)
				val->val.intValue = val->val.doubleValue;
			else if (val->type == CHAR)
				val->val.intValue = val->val.stringValue[0];
			else
			{
				errorHandler.AddError("Wrong type, expected: " + std::to_string(expectedType), token, true);
				return false;
			}
			break;

		case CHAR:

			if (val->type == CHAR);
			else if (val->type == FLOAT)
				val->val.intValue = val->val.doubleValue;
			else if (val->type == INTEGER)
				val->val.stringValue[0] = (char)val->val.intValue;
			else
			{
				errorHandler.AddError("Wrong type, expected: " + std::to_string(expectedType), token, true);
				return false;
			}
			val->type = expectedType;
			break;

		case FLOAT:

			if (val->type == FLOAT);
			else if (val->type == INTEGER)
				val->val.doubleValue = val->val.intValue;
			else if (val->type == CHAR)
				val->val.doubleValue = val->val.stringValue[0];
			else
			{
				errorHandler.AddError("Wrong type, expected: " + std::to_string(expectedType), token, true);
				return false;
			}
			break;

		default:
			if (val->type != expectedType)
			{
				errorHandler.AddError("Wrong type, expected: " + std::to_string(expectedType), token, true);
				return false;
			}
			break;
		}

		return true;
	}


	void AcceptToken() { tokenAccepted = true; }

	//Non-terminals
	void Program();
	void ProgramHeader();
	void ProgramBody();
	void Declaration();
	void ProcDec();
	void ProcHeader();
	std::vector<Parameter> ParamList();
	Parameter Param();
	void ProcBody();
	Parameter VarDec(bool isParam);
	int TypeMark();
	void Statement();
	void LowerBound();
	void UpperBound();
	void ProcCall(std::string lastToken);
	void AssignState(std::string lastToken);
	void Destination();
	void IfState();
	void LoopState();
	void ReturnState();
	Value Expr(int);
	Value ArithOp(int);
	Value Relation(int);
	Value Term(int);
	Value Factor(int);
	Value Name();
	void Number();
	std::vector<Value> ArgList(std::vector<Parameter>);


	//Non-Terminal' to remove left recursion
	Value Expr_(int,Value);
	Value ArithOp_(int,Value);
	Value Relation_(int, Value);
	Value Term_(int, Value);

};





#endif