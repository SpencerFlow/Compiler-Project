
#ifndef SCANNER_H
#define SCANNER_H

#include <map>
#include "definitions.h"
#include <stdlib.h>

class Scanner
{
	private:
		std::map<std::string, int> reservedTable;
		std::map<std::string, int>::iterator it;
		std::ifstream *infile;
		int lineNumb;
		bool ClearSpace(char* inChar);

	public:
		Scanner();
		~Scanner();
		token_t ScanOneToken();
		bool InitScanner(std::string filePath);

};


#endif