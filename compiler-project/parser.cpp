#include "parser.h"
#include <iostream>
#include <cstdio>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Non-Terminals
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void Parser::Program()
{
	//printf("Program");
	ProgramHeader();
		
	//Add scope to symbol table
	stManager.AddScope();
	std::ostringstream *procStream = new std::ostringstream;
	procStreamVector.push_back(procStream);

	///////////////////////////////////// CODE GEN ////////////////////////////////////////////////
	assemblyFile << "declare void @putinteger(i32)" << std::endl;
	assemblyFile << "declare void @putbool(i1)" << std::endl;
	assemblyFile << "declare void @putfloat(i32)" << std::endl;
	assemblyFile << "declare void @putstring(i8*)" << std::endl;
	assemblyFile << "declare void @putcharacter(i8)" << std::endl << std::endl;
	assemblyFile << "declare void @getinteger(i32*)" << std::endl << std::endl;
	assemblyFile << "declare void @getcharacter(i8*)" << std::endl << std::endl;
	///////////////////////////////////// CODE GEN ////////////////////////////////////////////////

	regCount = 0;
	ProgramBody(); 

	//Remove scope from symbol table
	stManager.RemoveScope();
	assemblyFile << procStreamVector.back()->str();
	delete procStreamVector.back();
	procStreamVector.pop_back();

	//If the program doesnt end in '.' add error
	if (GetToken().type != '.') errorHandler.AddError(token, '.', true); 
		
}
void Parser::ProgramHeader()
{
	//std::cout<<"ProgramHeader"<<std::endl;
	if(GetToken().type != PROGRAM) errorHandler.AddError(token, PROGRAM, true);
	AcceptToken();
	if(GetToken().type != IDENTIFIER) errorHandler.AddError(token, IDENTIFIER, true);
	AcceptToken();
	if(GetToken().type != IS) errorHandler.AddError(token, IS, true);
	AcceptToken();
}
void Parser::ProgramBody()
{
	//std::cout<<"ProgramBody"<<std::endl;

	//CodeGen
	*procStreamVector.back() << "\ndefine i32 @main() {" << std::endl;
	regCount++;

	while(GetToken().type != BEGIN)
	{
		Declaration();

		if(GetToken().type != ';') errorHandler.AddError(token, ';',true);
		AcceptToken();
	}
	AcceptToken();

	regCount = ++mainRegCount;

	while(GetToken().type != END)
	{
		Statement();

		if (GetToken().type != ';') errorHandler.AddError(token, ';', true);
		AcceptToken();
	}

	AcceptToken();
	if(GetToken().type != PROGRAM) errorHandler.AddError(token, PROGRAM,true);

	//Code gen
	*procStreamVector.back() << "    ret i32 0\n}" << std::endl;

	AcceptToken();
}
void Parser::ProcBody()
{
	
	//std::cout<<"ProcBody"<<std::endl;
	while(GetToken().type != BEGIN)
	{
		Declaration();
		if (GetToken().type != ';') errorHandler.AddError(token, ';', true);
		AcceptToken();
	}
	AcceptToken();

	while(GetToken().type != END)
	{
		Statement();
		if (GetToken().type != ';') errorHandler.AddError(token, ';', true);
		AcceptToken();
	}
	AcceptToken();

	if(GetToken().type != PROCEDURE)
	{
		errorHandler.AddError(token, PROCEDURE,true);
	}

	//Code gen
	*procStreamVector.back() << "    ret void\n}\n";

	AcceptToken();

}
void Parser::Declaration()
{
	//std::cout<<"Declaration"<<std::endl;
	isGlobal = false;
	if (GetToken().type == GLOBAL)
	{
		if(stManager.getCurrentScope() != 0)
		{
			errorHandler.AddWarning("Cannot declare GLOBAL inside of Procedure. Assuming local.", GetToken(), true);
			isGlobal = false;
		}
		else
		{
			isGlobal = true;
		}
		AcceptToken();
	} 
	
	if(GetToken().type == PROCEDURE)
	{
		ProcDec();
	}
	else if(GetToken().type == R_INTEGER || GetToken().type == R_FLOAT || GetToken().type == R_BOOL || GetToken().type == R_CHAR)
	{	
		VarDec(false);
	}
	else
	{
		errorHandler.AddError(token, PROCEDURE, true);
		AcceptToken();
	}

	

}
void Parser::ProcDec()
{
	//std::cout<<"ProcDec"<<std::endl;
	ProcHeader();
	//GetToken();
	ProcBody();
	stManager.RemoveScope();

	assemblyFile << procStreamVector.back()->str();
	delete procStreamVector.back();
	procStreamVector.pop_back();
		
}
void Parser::ProcHeader()
{
	std::ostringstream *procStream = new std::ostringstream;
	procStreamVector.push_back(procStream);

	std::vector<Parameter> paramTypes;
	//std::cout<<"ProcHeader"<<std::endl;
	if(GetToken().type != PROCEDURE) errorHandler.AddError(token, PROCEDURE,true);
	AcceptToken();
	if(GetToken().type != IDENTIFIER) errorHandler.AddError(token, IDENTIFIER,true);

	//Code Gen
	*procStreamVector.back() << "\ndefine void @" << GetToken().val.stringValue << "(";
	//regCount++;
	
	std::string procName = GetToken().val.stringValue;

	stManager.AddScope();
	regCount = 0;
	isGlobal = false;

	AcceptToken();

	if(GetToken().type != LPAREN)
		errorHandler.AddError(token, LPAREN, true);

	AcceptToken();
	if(GetToken().type != ')')
		paramTypes = ParamList();

	if(GetToken().type != RPAREN)
		errorHandler.AddError(token, RPAREN, true);

	stManager.AddDeclaration(procName, PROC_TYPE, regCount++, paramTypes, isGlobal);

	//Code gen
	*procStreamVector.back() << ") {\n";

	AcceptToken();

}
std::vector<Parameter> Parser::ParamList()
{
	std::vector<Parameter> paramTypes;

	paramTypes.push_back(Param());
	
	while (GetToken().type == ',')
	{
		//Code gen
		*procStreamVector.back() << ",";
		AcceptToken();
		paramTypes.push_back(Param());
	}

	return paramTypes;

}
Parameter Parser::Param()
{
	//std::cout<<"Param"<<std::endl;
	Parameter param;
	bool isParam = true;
	param = VarDec(isParam);

	AcceptToken();

	return param;

}
Parameter Parser::VarDec(bool isParam)
{
	//std::cout<<"VarDec"<<std::endl;
	int type = TypeMark();
	Parameter param;
	param.mode = 0;

	std::vector<Parameter> params;

	std::string varName = GetToken().val.stringValue;

	switch (type)
	{
	case R_INTEGER:
		type = INTEGER;
		break;
	case R_FLOAT:
		type = FLOAT;
		break;
	case R_BOOL:
		type = BOOL;
		break;
	case R_CHAR:
		type = CHAR;
		break;
	case R_STRING:
		type = STRING;
		break;
	default:
		break;
	}

	param.type = type;
	std::string llvmType = "";
	std::string namedReg = "";

	if (GetToken().type != IDENTIFIER)
		errorHandler.AddError(token, IDENTIFIER, true);

	namedReg = token.val.stringValue;

	AcceptToken();

	if (GetToken().type == '[')
	{
		AcceptToken();
		Number();

		if (GetToken().type != ':')
			errorHandler.AddError(token, ':', true);
		AcceptToken();

		Number();
		if (GetToken().type != ']')
			errorHandler.AddError(token, ']', true);

		AcceptToken();
	}



	///////////////////////////////////// CODE GEN ////////////////////////////////////////////////

	llvmType = typeToAssembly(type);

	if (isParam)
	{
		*procStreamVector.back() << llvmType;

		if (GetToken().type == IN)
		{
			param.mode = GetToken().type;
		}
		else if (GetToken().type == OUT)
		{
			*procStreamVector.back() << "*";
			param.mode = GetToken().type;
		}
		else if (GetToken().type == INOUT)
		{
			*procStreamVector.back() << "*";
			param.mode = GetToken().type;
		}

		params.push_back(param);
		stManager.AddDeclaration(varName, type, regCount, params, isGlobal);

		AcceptToken();
		regCount++;
	}
	else
	{
		params.push_back(param);
		stManager.AddDeclaration(varName, type, regCount, params, isGlobal);
		if (isGlobal)
		{
			assemblyFile << "@";
			assemblyFile << namedReg << " = " << "common global " << llvmType << " 0" << "\n";
		}
		else
		{
			if (procStreamVector.size() == 1)
				mainRegCount++;

			*procStreamVector.back() << "    \%" << regCount << " = alloca " << llvmType << "\n";
			regCount++;
		}
	}


	isGlobal = false;
	return param;

}
int Parser::TypeMark()
{
	//std::cout<<"TypeMark"<<std::endl;
	int tokenType = 0;

	if (GetToken().type == R_INTEGER || GetToken().type == R_CHAR || GetToken().type == R_BOOL || GetToken().type == R_FLOAT)
	{
		tokenType = GetToken().type;
	}
	else
	{
		errorHandler.AddError(token, R_INTEGER, true);
	}

	AcceptToken();
	return tokenType;

}
void Parser::Number()
{
	//std::cout<<"Number"<<std::endl;
	if(GetToken().type != FLOAT && GetToken().type != INTEGER)
		errorHandler.AddError(token, INTEGER, true);
	

	AcceptToken();
}
void Parser::Statement()
{
	
	if(GetToken().type == IDENTIFIER)
	{
		std::string lastToken = GetToken().val.stringValue;
		AcceptToken();
		if (GetToken().type == '[' || GetToken().type == ASSIGN)
		{
			AssignState(lastToken);
		}
		else if (GetToken().type == '(')
		{
			ProcCall(lastToken);
		}
		else
		{
			errorHandler.AddError(token, '(', true);
		}

	}
	else if (GetToken().type == BUILTIN)
	{
		Value val;
		std::string builtin = GetToken().val.stringValue;
		AcceptToken();

		if (GetToken().type != '(')
		{
			errorHandler.AddError(token, '(', true);
			return;
		}

		AcceptToken();
		if (builtin == "getbool")
		{
			val = Expr(BOOL);
			
		}
		else if (builtin == "getinteger")
		{
			val = Expr(INTEGER);
			*procStreamVector.back() << "    call void @" << builtin;
			*procStreamVector.back() << "(" << typeToAssembly(val.type) << "* %" << val.addrReg << ")" << std::endl;
		}
		else if (builtin == "getfloat")
		{
			val = Expr(FLOAT);
		}
		else if (builtin == "getstring")
		{
			val = Expr(STRING);
		}
		else if (builtin == "getcharacter")
		{
			val = Expr(CHAR);
			*procStreamVector.back() << "    call void @" << builtin;
			*procStreamVector.back() << "(" << typeToAssembly(val.type) << "* %" << val.addrReg << ")" << std::endl;
		}
		else if (builtin == "putbool")
		{
			val = Expr(BOOL);
		}
		else if (builtin == "putinteger")
		{
			val = Expr(INTEGER);
			*procStreamVector.back() << "    call void @" << builtin;
			*procStreamVector.back() << "(" << typeToAssembly(val.type) << " %" << val.reg << ")" << std::endl;
		}
		else if (builtin == "putfloat")
		{
			val = Expr(FLOAT);
		}
		else if (builtin == "putstring")
		{
			val = Expr(STRING);
		}
		else if (builtin == "putcharacter")
		{
			val = Expr(INTEGER);
			*procStreamVector.back() << "    call void @" << builtin;
			*procStreamVector.back() << "(" << typeToAssembly(val.type) << " %" << val.reg << ")" << std::endl;
		}

		if (GetToken().type != ')')
			errorHandler.AddError(token, ')', true);

		AcceptToken();

	}
	else if(GetToken().type == IF)
	{
		IfState();
	}
	else if(GetToken().type == FOR)
	{
		LoopState();
	}
	else if(GetToken().type == RETURN)
	{
		ReturnState();
	}
	else
	{
		errorHandler.AddError(token, IDENTIFIER,true);
		AcceptToken();
		
	}

}
/*void Parser::LowerBound()
{

}
void Parser::UpperBound()
{

}*/
void Parser::ProcCall(std::string procName)
{
	std::vector<Value> argList;
	Value val = stManager.IsDeclared(procName);

	AcceptToken();
	if (GetToken().type != ')')
		argList = ArgList(val.paramList);

	if(GetToken().type != ')')
		errorHandler.AddError(token, ')',true);
		
	AcceptToken();

	///////////////////////////////Code Gen////////////////////////////////////////
	*procStreamVector.back() << "    call void @" << procName;
	*procStreamVector.back() << "(";
	if(!argList.empty())
	{
		int it = 0;
		int paramVal;
		for (auto & arg : argList) 
		{

			std::string llvmString;
			if (val.paramList[it].mode == OUT || val.paramList[it].mode == INOUT)
			{
				llvmString = typeToAssembly(arg.type) + "*";
				paramVal = arg.addrReg;
			}
			else
			{
				llvmString = typeToAssembly(arg.type);
				paramVal = arg.reg;
			}

    		*procStreamVector.back() <<  llvmString << " ";
			if(arg.isIdentifier == true)
			{
				*procStreamVector.back() << "%" << paramVal;
			}
			else if(arg.type == INTEGER || arg.type == BOOL)
			{
				*procStreamVector.back() << arg.val.intValue;
			}
			else if(arg.type == FLOAT)
			{
				*procStreamVector.back() << arg.val.doubleValue;
			}
			else if(arg.type == CHAR)
			{
				*procStreamVector.back() << arg.val.stringValue[0];
			}
			else if(arg.type == STRING)
			{
				*procStreamVector.back() << arg.val.stringValue;
			}

			if(it != argList.size() -1)
			{
				*procStreamVector.back() << ",";
			}

			it++;
		}
	}
	*procStreamVector.back() << ")" << std::endl;
}
void Parser::AssignState(std::string lastToken)
{	
	Value destination = stManager.IsDeclared(lastToken);

	if(destination.paramList[0].mode == IN)
		errorHandler.AddError("Can't assign to an IN parameter",token, true);

	Value expression;

	Destination();
	if(GetToken().type != ASSIGN)
		errorHandler.AddError(token, ASSIGN,true);

	AcceptToken();
	expression = Expr(destination.type);

	//////////////////////////////////////////////Code-Gen///////////////////////////////////////////////////
	*procStreamVector.back() << "    store ";

	if (!expression.isIdentifier)
	{
		*procStreamVector.back() << typeToAssembly(expression.type) << " " << typeToStringVal(expression) << ", ";
	}
	else
	{
		*procStreamVector.back() << typeToAssembly(expression.type) << " %" << expression.reg << ", ";
	}

	if(destination.isGlobal)
	{
		*procStreamVector.back() << typeToAssembly(destination.type) << "* @" << destination.name << "\n";
	}
	else
	{
		*procStreamVector.back() << typeToAssembly(destination.type) << "* %" << destination.reg << "\n";
	}

	//////////////////////////////////////////////Code-Gen///////////////////////////////////////////////////

}
void Parser::Destination()
{
	//AcceptToken();
	if(GetToken().type == '[')
	{
		AcceptToken();
		Expr(INTEGER);

		if(GetToken().type != ']')
			errorHandler.AddError(token, ']',true);

		AcceptToken();
	}
}
void Parser::IfState()
{
	Value val;

	int ifLabel = labelCount++;
	int elseLabel = labelCount++;
	int endLabel = labelCount++;

	////std::cout<<"IfState"<<//std::endl;
	if(GetToken().type != IF)
		errorHandler.AddError(token, IF,true);

	AcceptToken();
	if(GetToken().type != LPAREN)
		errorHandler.AddError(token, LPAREN,true);

	AcceptToken();
	val = Expr(BOOL);

	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////

	*procStreamVector.back() << "    br " << typeToAssembly(val.type);

	if (val.isIdentifier)
		*procStreamVector.back() << " %" << val.reg;
	else
		*procStreamVector.back() << " " << typeToStringVal(val);

	*procStreamVector.back() << ", label %L" << ifLabel << ", label %L" << elseLabel << "\n"<< std::endl;

	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////

	if(GetToken().type != RPAREN)
		errorHandler.AddError(token, RPAREN,true);

	AcceptToken();
	if(GetToken().type != THEN)
		errorHandler.AddError(token, THEN,true);

	AcceptToken();
	if(GetToken().type == ELSE || GetToken().type == END)
	{
		errorHandler.AddError(token, IDENTIFIER,true);	
	}
	else
	{	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
		*procStreamVector.back() << "L" << ifLabel << ":" << std::endl;
		while (GetToken().type != ELSE && GetToken().type != END)
		{
			Statement();
			if (GetToken().type != ';')
				errorHandler.AddError(token, ';', true);
			AcceptToken();
		}
		/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
		*procStreamVector.back() << "    br label %L" << endLabel << "\n" << std::endl;

	}

	if(GetToken().type == ELSE)
	{
		AcceptToken();
		if(GetToken().type == END)
		{
			errorHandler.AddError(token, IDENTIFIER,true);
		}
		else
		{
			/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
			*procStreamVector.back() << "L" << elseLabel << ":" << std::endl;
			while(GetToken().type != END)
			{
				Statement();
				if(GetToken().type != ';')
					errorHandler.AddError(token, ';', true);
				AcceptToken();
			}
			/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
			*procStreamVector.back() << "    br label %L" << endLabel << "\n" << std::endl;
		}


	}

	if(GetToken().type != END)
		errorHandler.AddError(token, END,true);

	AcceptToken();
	if(GetToken().type != IF)
		errorHandler.AddError(token, IF,true);

	*procStreamVector.back() << "L" << endLabel << ":" << std::endl;

	AcceptToken();

}
void Parser::LoopState()
{
	int loopLabel0 = labelCount++;
	int loopLabel1 = labelCount++;
	int endLabel = labelCount++;
	
	Value val;

	if(GetToken().type != FOR)
		errorHandler.AddError(token, FOR,true);

	AcceptToken();
	if(GetToken().type != LPAREN)
		errorHandler.AddError(token, LPAREN,true);


	AcceptToken();

	if (GetToken().type == IDENTIFIER)
	{
		std::string lastToken = token.val.stringValue;
		AcceptToken();
		GetToken();
		AssignState(lastToken);
	}
	else
	{
		errorHandler.AddError(token, IDENTIFIER, true);
	}

	if (GetToken().type != ';')
		errorHandler.AddError(token, ';',true);

	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
	*procStreamVector.back() << "    br label %L" << loopLabel0 << "\n" << std::endl;
	*procStreamVector.back() << "L" << loopLabel0 << ":" << std::endl;

	AcceptToken();
	val = Expr(BOOL);

	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
	*procStreamVector.back() << "    br " << typeToAssembly(val.type);
	if (val.isIdentifier)
		*procStreamVector.back() << " %" << val.reg;
	else
		*procStreamVector.back() << " " << typeToStringVal(val);
	*procStreamVector.back() << ", label %L" << loopLabel1 << ", label %L" << endLabel << "\n" << std::endl;


	if(GetToken().type != RPAREN)
		errorHandler.AddError(token, RPAREN,true);
	
	AcceptToken();

	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
	*procStreamVector.back() << "L" << loopLabel1 << ":" << std::endl;

	while(GetToken().type != END)
	{
		Statement();
		if(GetToken().type != ';')
			errorHandler.AddError(token, ';', true);
		AcceptToken();
	}

	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
	*procStreamVector.back() << "    br label %L" << loopLabel0 << "\n" << std::endl;


	if(GetToken().type != END)
		errorHandler.AddError(token, END,true);

	AcceptToken();
	if(GetToken().type != FOR)
		errorHandler.AddError(token, FOR,true);

	/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
	*procStreamVector.back() << "L" << endLabel << ":" << std::endl;

	AcceptToken();

}
void Parser::ReturnState()
{
	//std::cout<<"ReturnState"<<//std::endl;
	if(GetToken().type != RETURN)
		errorHandler.AddError(token, RETURN,true);
	AcceptToken();
}
Value Parser::ArithOp(int expectedType)
{
	Value arithVal;

	arithVal = Relation(expectedType);
	return ArithOp_(expectedType, arithVal);
}
Value Parser::Relation(int expectedType)
{
	Value relVal;

	//std::cout<<"Relation"<<//std::endl;
	relVal = Term(expectedType);
	return Relation_(expectedType, relVal);
}

Value Parser::Name()
{
	//std::cout<<"Name"<<//std::endl;
	if(GetToken().type != IDENTIFIER)
		errorHandler.AddError(token, IDENTIFIER,true);

	Value symbolVal = stManager.IsDeclared(GetToken().val.stringValue);

	if (symbolVal.paramList[0].mode == INOUT || 0)
	{
		//Code Gen
		*procStreamVector.back() << "    %" << regCount << " = load " << typeToAssembly(symbolVal.type) << ", "
			<< typeToAssembly(symbolVal.type) << "* ";

		if (symbolVal.isGlobal)
		{
			*procStreamVector.back() << "@" << symbolVal.name << "\n";
		}
		else
		{
			*procStreamVector.back() << "%" << symbolVal.reg << "\n";
		}
		symbolVal.reg = regCount++;
	}
	else if(symbolVal.paramList[0].mode == OUT)
	{
		errorHandler.AddError("Can't use OUT param in expressions",token, true);
	}

	AcceptToken();
	if(GetToken().type == '[')
	{
		AcceptToken();
		Expr(INTEGER);
		
		if (GetToken().type != ']')
			errorHandler.AddError(token, ']', true);
		
		AcceptToken();
	}

	return symbolVal;
}

std::vector<Value> Parser::ArgList(std::vector<Parameter> typeList)
{
	std::vector<Value> argList;
	int it = 0;

	if (typeList.empty())
		return argList;

	argList.push_back(Expr(typeList.at(it++).type));
	

	while(GetToken().type == ',')
	{
		AcceptToken();
		argList.push_back(Expr(typeList.at(it++).type));
	}

	return argList;
}

Value Parser::Expr(int expectedType)
{
	Value expVal;

	if (GetToken().type == NOT)
	{
		AcceptToken();
		expVal = ArithOp(expectedType);
		expVal.val.intValue = !expVal.val.intValue;
	}
	else
	{
		expVal = ArithOp(expectedType);
	}

	expVal = Expr_(expectedType, expVal);

	if (!TypeConvert(expectedType, &expVal))
		return expVal;

	return expVal;
}

Value Parser::Term(int expectedType)
{
	Value termVal;

	//std::cout<<"Term"<<std::endl;
	termVal = Factor(expectedType);
	return Term_(expectedType, termVal);
}

Value Parser::Factor(int expectedType)
{

	Value returnVal;
	returnVal.isIdentifier = false;

	if(GetToken().type == INTEGER || FLOAT || BOOL || CHAR || STRING)
		returnVal.type = GetToken().type;

	if(GetToken().type == '(')
	{
		AcceptToken();
		Expr(expectedType);

		if(GetToken().type != ')')
			errorHandler.AddError(token, ')',true);

		AcceptToken();

	}
	else if(GetToken().type == '-')
	{
		AcceptToken();

		if(GetToken().type == IDENTIFIER)
		{
			//TODO: Make negative
			returnVal.type = GetToken().type;
			returnVal = Name();
			returnVal.isIdentifier = true;
		}
		else if(GetToken().type == FLOAT)
		{
			returnVal.val.doubleValue = -1 * GetToken().val.doubleValue;
			returnVal.type = GetToken().type;
			Number();
		}
		else if(GetToken().type == INTEGER)
		{
			returnVal.val.intValue = -1 * GetToken().val.intValue;
			returnVal.type = GetToken().type;
			Number();
		}
		else
		{
			errorHandler.AddError(token, IDENTIFIER,true);
			AcceptToken();
		}

	}
	else if(GetToken().type == IDENTIFIER)
	{
		returnVal = Name();
		returnVal.isIdentifier = true;
	}
	else if(GetToken().type == FLOAT)
	{
		returnVal.val.doubleValue = GetToken().val.doubleValue;
		Number();
	}
	else if(GetToken().type == INTEGER)
	{
		returnVal.val.intValue = GetToken().val.intValue;
		Number();
	}
	else if(GetToken().type == STRING)
	{
		strcpy(returnVal.val.stringValue, GetToken().val.stringValue);
		AcceptToken();
	}
	else if(GetToken().type == CHAR)
	{
		strcpy(returnVal.val.stringValue, GetToken().val.stringValue);
		AcceptToken();
	}
	else if(GetToken().type == BOOL)
	{
		returnVal.val.intValue = GetToken().val.intValue;
		returnVal.type = GetToken().type;
		AcceptToken();
	}
	else
	{
		errorHandler.AddError(token, IDENTIFIER,true);
		AcceptToken();
	}

	return returnVal;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///Non-Terminals Prime Functions to remove Left Recusion
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Value Parser::Expr_(int expectedType, Value val)
{
	//if (!TypeConvert(&expectedType, &val))
		//return val;
	Value rhs;

	if (GetToken().type == '&')
	{
		AcceptToken();

		rhs = ArithOp(expectedType);
		TypeConvert(val.type, &rhs);

		// Code Gen
		*procStreamVector.back() << "    %" << regCount << " = and ";
		if (!val.isIdentifier)
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << typeToStringVal(val) << ", ";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << " %" << val.reg << ", ";
		}

		if (!rhs.isIdentifier)
		{
			*procStreamVector.back() << typeToAssembly(rhs.type);
			*procStreamVector.back() << typeToStringVal(rhs) << std::endl;
			rhs.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(rhs.type);
			*procStreamVector.back() << " %" << rhs.reg << std::endl;
		}

		val.reg = regCount++;
		return Expr_(expectedType, val);

	}
	else if (GetToken().type == '|')
	{
		AcceptToken();

		rhs = ArithOp(expectedType);
		TypeConvert(val.type, &rhs);

		// Code Gen
		*procStreamVector.back() << "    %" << regCount << " = or ";

		if (!val.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(val) << ", ";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << " %" << val.reg << ", ";
		}

		if (!rhs.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(rhs) << std::endl;
			rhs.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(rhs.type);
			*procStreamVector.back() << " %" << rhs.reg << std::endl;
		}

		val.reg = regCount++;
		return Expr_(expectedType, val);
	}

	return val;

}

Value Parser::Term_(int expectedType, Value val)
{

	Value rhs;

	if(GetToken().type == '*')
	{
		AcceptToken();

		rhs = Factor(expectedType);
		TypeConvert(val.type, &rhs);

		/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
		


		*procStreamVector.back() << "    %" << regCount << " = mul nsw ";
        
		if (!val.isIdentifier)
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << " " << typeToStringVal(val) << ", ";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << " %" << val.reg << ", ";
		}

		if (!rhs.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(rhs) << "\n";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << "%" << rhs.reg << "\n";
		}
        /////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////

		val.reg = regCount++;
		return Term_(expectedType, val);
	}
	else if(GetToken().type == '/')
	{
		AcceptToken();

		rhs = Factor(expectedType);
		TypeConvert(val.type, &rhs);

		/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
		*procStreamVector.back() << "    %" << regCount << " = sdiv nsw ";
        
		if (!val.isIdentifier)
		{
			*procStreamVector.back() << typeToAssembly(val.type) << " ";
			*procStreamVector.back() << typeToStringVal(val) << ", ";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << " %" << val.reg << ", ";
		}

		if (!rhs.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(rhs) << "\n";
			val.isIdentifier = true;
		}
		else
		{
			
			*procStreamVector.back() << " %" << rhs.reg << "\n";
		}
        /////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////

		val.reg = regCount++;
		return Term_(expectedType, val);
	}

	return val;

}


Value Parser::ArithOp_(int expectedType, Value val)
{
	Value rhs;

	if (GetToken().type == '+')
	{
		AcceptToken();

		rhs = Relation(expectedType);
		TypeConvert(val.type, &rhs);

		/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
		*procStreamVector.back() << "    %" << regCount << " = add nsw ";
        
		if (!val.isIdentifier)
		{
			*procStreamVector.back() << typeToAssembly(val.type) << " ";
			*procStreamVector.back() << typeToStringVal(val) << ", ";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << " %" << val.reg << ", ";
		}

		if (!rhs.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(rhs) << "\n";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << "%" << rhs.reg << "\n";
		}
        /////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////

		val.reg = regCount++;
		return ArithOp_(expectedType, val);
	}
	else if (GetToken().type == '-')
	{
		AcceptToken();

		rhs = Relation(expectedType);
		TypeConvert(val.type, &rhs);

		/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
		*procStreamVector.back() << "    %" << regCount << "= sub nsw ";
        
		if (!val.isIdentifier)
		{
			*procStreamVector.back() << typeToAssembly(val.type) << " ";
			*procStreamVector.back() << typeToStringVal(val) << ", ";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << typeToAssembly(val.type);
			*procStreamVector.back() << " %" << val.reg << ", ";
		}

		if (!rhs.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(rhs) << "\n";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << " %" << rhs.reg << "\n";
		}
        /////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////

		val.reg = regCount++;
		return ArithOp_(expectedType, val);
	}

	return val;
}

Value Parser::Relation_(int expectedType, Value val)
{
	Value rhs;
	std::string llvmCode;

	if(GetToken().type == GE || GetToken().type == LE || GetToken().type == EQUAL|| GetToken().type == NE || GetToken().type == '>' || GetToken().type == '<')
	{
		switch(GetToken().type)
		{
			case GE:
				llvmCode = "icmp sge ";
				break;
			case LE:
				llvmCode = "icmp sle ";
				break;
			case EQUAL:
				llvmCode = "icmp eq ";
				break;
			case NE:
				llvmCode = "icmp ne ";
				break;
			case '>':
				llvmCode = "icmp sgt ";
				break;
			case '<':
				llvmCode = "icmp slt ";
				break;

		}

		AcceptToken();
		rhs = Term(expectedType);

		/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////
		*procStreamVector.back() <<"    %" << regCount << " = " << llvmCode << typeToAssembly(val.type);
		//<< " %" << val.reg << ", " << "%" << rhs.reg << std::endl;
		
		if (!val.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(val) << ", ";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << " %" << val.reg << ", ";
		}

		if (!rhs.isIdentifier)
		{
			*procStreamVector.back() << typeToStringVal(rhs) << "\n";
			val.isIdentifier = true;
		}
		else
		{
			*procStreamVector.back() << " %" << rhs.reg << "\n";
		}

		/////////////////////////////////////////////////Code-Gen/////////////////////////////////////////////////


		val.reg = regCount++;
		val.type = BOOL;

		return Relation_(expectedType, val);
	}

	return val;
 }


