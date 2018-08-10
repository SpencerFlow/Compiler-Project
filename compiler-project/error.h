
#ifndef ERROR
#define ERROR

#include "definitions.h"
#include <iostream>
#include <vector>
#include <stdio.h>
#include "scanner.h"

enum error_type { NO_ERR, INVALID_CHAR, CUSTOM, INVALIDTOKEN };

class Error
{
	public:
		// Constructors                                                                     
		Error() { Type = NO_ERR; Message = ""; }
		Error(error_type type) { Type = type; SetMessage(type); }
		Error(std::string msg) { Type = CUSTOM; Message = msg; }
		Error(token_t inToken, int expectedToken) 
		{ 
			
			SetMessage(error_type(INVALIDTOKEN));
			token = inToken;
			expected = expectedToken;
		}
		Error(token_t inToken, std::string msg)
		{
			Message = msg;
			token = inToken;
		}

		// Operator Overloading                                                             
		explicit operator bool() const { return Type != NO_ERR; }
		bool operator !() const { return Type == NO_ERR; }
		void operator ()(error_type type) { Type = type; SetMessage(type); }

		// Public Methods                                                                   
		void ChangeMessage(std::string msg) { Type = CUSTOM; Message = msg; }
		std::string GetErrMessage() { return Message; }
		void DisplayError() { std::cout << "\033[1;31mError: \033[0m"<<"[" <<token.lineNumb <<"] "<< Message << std::endl; }
		void DisplayWarning() { std::cout << "\033[1;33mWarning: \033[0m"<<"[" <<token.lineNumb <<"] "<< Message << std::endl; }
		void ParseDisplay() 
		{ 
			std::cout << "\033[1;31mError: \033[0m";
			std::cout << "[" << token.lineNumb << "] ";
			std::cout << Message;
			if (expected >= INTEGER)
			{
				std::cout << "Expected: " << multi_tokens(expected) << " ";
			}
			else
			{
				std::cout << "Expected: " << static_cast<char>(expected) << " ";
			}

			if (token.type < INTEGER)
			{
				std::cout << "Got: " << static_cast<char>(token.type) << std::endl;;
			}
			else
			{
				std::cout << "Got: " << multi_tokens(token.type) << std::endl;;
			}


		}


	private:

		error_type Type;
		token_t token;
		int expected;
		std::string Message;
		int lineNum;
		void SetMessage(error_type type)
		{
			switch (type)
			{

				case NO_ERR: Message = "";
				default:;
			}
		}
		void SetMessage(error_type type, token_t token, int expected)
		{
			switch (type)
			{
				case INVALIDTOKEN: Message = "Invalid Token: ";
				case NO_ERR: Message = "";
				default:;
			}
		}

};

class ErrorHandler
{
	private:
		int lineCount;
		std::vector<Error> errorList;
		Scanner *scanner;
	public:
		// Constructors                                                                     
		ErrorHandler() 
		{
			lineCount = 0;
		}
		void SetScanner(Scanner *scan)
		{
			scanner = scan;
		}
		void SetLineCount()
		{
			lineCount += 1;
		}
		void AddError(std::string message, token_t token, bool print)
		{
			Error error(token, message);
			errorList.push_back(error);
			if (print)
				error.DisplayError();
		}
		void AddError(std::string message, bool print)
		{
			Error error(message);
			errorList.push_back(error);
			if (print)
				error.DisplayError();
		}
		void AddWarning(std::string message, token_t token, bool print)
		{
			Error warning(token, message);
			errorList.push_back(warning);
			if (print)
				warning.DisplayWarning();
		}
		
		void AddError(token_t token, int expected, bool print)
		{
			token_t newToke;
			//newToke = scanner->ScanOneToken();
			Error error(token, expected);
			errorList.push_back(error);
			if (print)
				error.ParseDisplay();
		}
		void PrintLastError()
		{
			Error currentError = errorList.back();
			currentError.ParseDisplay();
		}


};




#endif