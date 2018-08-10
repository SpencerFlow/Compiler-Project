
#include <fstream>
#include "scanner.h"
#include <cstring>


Scanner::Scanner()
{
	infile = new std::ifstream;
	lineNumb = 1;
}

Scanner::~Scanner()
{
	infile->close();
	delete infile;
}

bool Scanner::ClearSpace(char* inChar)
{
	int tokenType;

	while (isspace(*inChar) || *inChar == '/')
	{
		if (*inChar == '\n')
		{
			lineNumb++;
		}

		if (infile->eof())
		{
			return true;
		}

		if (*inChar == '/')
		{
			infile->get(*inChar);

			if (*inChar == '/')
			{
				do
				{
					infile->get(*inChar);
				} while (*inChar != '\n');
				//lineNumb++;
			}
			else if (*inChar == '*')
			{
				int count = 1;
				while (count > 0)
				{
					infile->get(*inChar);
					if (*inChar == '/')
					{
						infile->get(*inChar);
						if (*inChar == '*')
						{
							count += 1;
						}
						else if (*inChar == '\n')
						{
							lineNumb++;
						}
					}
					else if (*inChar == '*')
					{
						infile->get(*inChar);
						if (*inChar == '/')
						{
							count -= 1;
						}
						else if (*inChar == '\n')
						{
							lineNumb++;
						}
					}
					else if (*inChar == '\n')
					{
						lineNumb++;
					}
				}
			}
		}
		else
		{
			infile->get(*inChar);
		}
	}
	return false;

}



bool Scanner::InitScanner(std::string filePath)
{
	infile->open(filePath.c_str());


	reservedTable["program"] = PROGRAM;
	reservedTable["procedure"] = PROCEDURE;
	reservedTable["global"] = GLOBAL;
	reservedTable["is"] = IS;
	reservedTable["begin"] = BEGIN;
	reservedTable["while"] = WHILE;
	reservedTable["float"] = FLOAT;
	reservedTable["integer"] = R_INTEGER;
	reservedTable["bool"] = R_BOOL;
	reservedTable["char"] = R_CHAR;
	reservedTable["string"] = R_STRING;
	reservedTable["for"] = FOR;
	reservedTable["if"] = IF;
	reservedTable["then"] = THEN;
	reservedTable["else"] = ELSE;
	reservedTable["end"] = END;
	reservedTable["true"] = BOOL;
	reservedTable["false"] = BOOL;
	reservedTable["not"] = NOT;
	reservedTable["in"] = IN;
	reservedTable["out"] = OUT;
	reservedTable["return"] = RETURN;
	reservedTable["getbool"] = BUILTIN;
	reservedTable["getinteger"] = BUILTIN;
	reservedTable["getfloat"] = BUILTIN;
	reservedTable["getstring"] = BUILTIN;
	reservedTable["getcharacter"] = BUILTIN;
	reservedTable["putbool"] = BUILTIN;
	reservedTable["putinteger"] = BUILTIN;
	reservedTable["putfloat"] = BUILTIN;
	reservedTable["putstring"] = BUILTIN;
	reservedTable["putcharacter"] = BUILTIN;



	if (infile->is_open())
	{
		return true;
	}
	else
	{
		return false;
	}
		
}

token_t Scanner::ScanOneToken()
{
	char c;
	token_t token;

	infile->get(c);

	if (ClearSpace(&c))
	{
		token.type = F_END;
		return token;
	}

	token.lineNumb = lineNumb;

	switch (c)
	{
		case '/':
	
			token.type = '/';
			return token;

		case '\'':
		{
			token.type = CHAR;
			infile->get(c);

			std::string character = "";
			character += c;
			strcpy(token.val.stringValue, character.c_str());

			infile->get(c);
			if (c != '\'')
			{
				//TODO: Error handling

				token.type = UNKNOWN;
				infile->putback(c);
			}

			return token;
		}

		case ':':
			infile->get(c);
			if (c == '=')
			{
				token.type = ASSIGN;
			}
			else
			{
				token.type = ':';
				infile->putback(c);
			}
			return token;

		case '=':
			infile->get(c);
			if (c == '=')
			{
				token.type = EQUAL;
			}
			else
			{
				token.type = UNKNOWN;
				infile->putback(c);
			}
			return token;

		case '!':
			infile->get(c);
			if (c == '=')
			{
				token.type = NE;
			}
			else
			{
				token.type = UNKNOWN;
				infile->putback(c);
			}
			return token;

		case '"':
		{
			token.type = STRING;
			std::string charString = "";
			while (infile->get(c) && c != '"')
			{
				charString += c;
			}
			infile->putback(c);

			//Add error handling

			strcpy(token.val.stringValue, charString.c_str());
			return token;
			
		}

		case '>':
			infile->get(c);
			if (c == '=')
			{
				token.type = GE;
			}
			else
			{
				token.type = '>';
				infile->putback(c);
			}
			return token;

		case '<':
			infile->get(c);
			if (c == '=')
			{
				token.type = LE;
			}
			else
			{
				token.type = '<';
				infile->putback(c);
			}
			return token;

		case '.': case'#': case',': case ';': case '(': case ')': case '*': case '+': 
		case '-': case '[': case ']': case '&': case '|':
			token.type = c;
			return token;

		case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		{
			std::string number = "";
			token.type = INTEGER;
			number += c;

			while (infile->get(c) && isdigit(c))
			{
				number += c;
			}

			if (c == '.')
			{
				token.type = FLOAT;
				number += c;
				while (infile->get(c) && isdigit(c))
				{
					number += c;
				}

				token.val.doubleValue = atof(number.c_str());
			}
			else
			{
				token.val.intValue = atoi(number.c_str());
			}

			infile->putback(c);
			return token;

		}

		default:

			token.type = UNKNOWN;
			if (!isalpha(c))
				return token;

			std::string str = "";
			do
			{
				if (isupper(c)) c = tolower(c);
				str += c;
				infile->get(c);

			} while (isalpha(c) || c == '_' || isdigit(c));

			strcpy(token.val.stringValue, str.c_str());
			infile->putback(c);
			token.type = IDENTIFIER;

			it = reservedTable.find(str);
			if (it != reservedTable.end())
			{
				//Set token type
				token.type = reservedTable[str];

				if (str == "false")
				{
					token.val.intValue = 0;
				}
				else if (str == "true")
				{
					token.val.intValue = 1;
				}
			}

			return token;

			
	}
}
