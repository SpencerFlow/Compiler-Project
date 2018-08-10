#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "error.h"
#include "definitions.h"

struct SymbolTableItem
{
	bool isGlobal;
	int type;
	int regNum;
	int addrReg;
	std::string name;
	int scopeLevel;
	std::vector<Parameter> paramTypes;
};


class SymbolTableManager
{
	private:
		std::vector<std::map<std::string, SymbolTableItem> > SymbolTableScopes;
		std::map<std::string, SymbolTableItem>::iterator it;
		int currentScope;
		ErrorHandler *errorHandler;

	public:

		SymbolTableManager()
		{
			currentScope = 0;
		}
		void SetErrorHandler(ErrorHandler *errors)
		{
			errorHandler = errors;
		}
		void AddScope()
		{
			if (SymbolTableScopes.size() > 0)
				currentScope++;
			std::map<std::string, SymbolTableItem> SymbolTable;
			SymbolTableScopes.push_back(SymbolTable);
		}


		void RemoveScope()
		{
			SymbolTableScopes.pop_back();
			currentScope--;
		}

		int getCurrentScope()
		{
			return currentScope;
		}

		//Put new identifier in symboltable for curent scope
		void AddDeclaration(std::string identifier, int type, int reg, std::vector<Parameter> paramTypes, bool global)
		{
			it = SymbolTableScopes[0].find(identifier);
			if (it != SymbolTableScopes[0].end() && SymbolTableScopes[0][identifier].isGlobal)			//If token identifier has been declared in the global scope with the GLOBAL prefix
			{
				errorHandler->AddError("multiply declared - " + identifier, true);
				// Throw "multiply declared variable" error
			}
			else																						//If identifier was not declared in the global scope
			{
				it = SymbolTableScopes.back().find(identifier);
				if (it != SymbolTableScopes.back().end())						//If token identifier has been declared in the local scope
				{
					errorHandler->AddError("multiply declared - " + identifier, true);
					// Throw "multiply declared variable" error
				}
				else														//If identifier was not declared in the local scope
				{
					SymbolTableItem newDeclaration;
					newDeclaration.scopeLevel = currentScope;
					newDeclaration.name = identifier;
					newDeclaration.isGlobal = global;
					newDeclaration.type = type;
					newDeclaration.regNum = reg;
					newDeclaration.addrReg = reg;
					newDeclaration.paramTypes = paramTypes;
					
							

					std::pair<std::map<std::string, SymbolTableItem>::iterator, bool > result;
					result = SymbolTableScopes.back().insert(
						std::pair<std::string, SymbolTableItem>(identifier, newDeclaration));

					if(type == PROC_TYPE)
					{
						SymbolTableScopes.end()[-2].insert(
							std::pair<std::string, SymbolTableItem>(identifier, newDeclaration));
					}
				}
				
			}
		}

		//Check if given identifier has been declared in the global or local scope 
		Value IsDeclared(std::string identifier)
		{
			Value symbolVal;
			symbolVal.type = UNKNOWN;
			//If token identifier has been declared in the scopes below the current scope
			it = SymbolTableScopes[currentScope].find(identifier);
			if (it != SymbolTableScopes[currentScope].end())
			{
				symbolVal.reg = it->second.regNum;
				symbolVal.type = it->second.type;
				symbolVal.paramList = it->second.paramTypes;
				symbolVal.addrReg = it->second.addrReg;
				symbolVal.name = it->second.name;
				symbolVal.isGlobal = it->second.isGlobal;
				
				return symbolVal;
			}

			//If identifier was not declared in the local scope
			it = SymbolTableScopes[0].find(identifier);
			//If token identifier has been declared in the global scope with the GLOBAL prefix
			if (it != SymbolTableScopes[0].end() && SymbolTableScopes[0][identifier].isGlobal)
			{
				symbolVal.reg = it->second.regNum;
				//sybolVal.val.stringValue = it->second.name;
				symbolVal.type = it->second.type;
				symbolVal.paramList = it->second.paramTypes;
				symbolVal.isGlobal = it->second.isGlobal;
				symbolVal.name = it->second.name;
				symbolVal.addrReg = it->second.addrReg;

				return symbolVal;
			}
			else
			{
				errorHandler->AddError("undeclared identifier - " + identifier, true);
				return symbolVal;
			}

		}
};


#endif