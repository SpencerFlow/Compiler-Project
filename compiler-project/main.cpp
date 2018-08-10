#include "scanner.h"
#include "parser.h"


int main(int argc, char** argv)
{
	std::string path = "";
	if(argc == 2 )
		path = argv[1];
	
	Scanner scanner;


	if(scanner.InitScanner(path))
	{
		Parser parser(&scanner);
		parser.Program();
	}
	else
	{
		std::cout << "\033[1;31mError: \033[0m" << "Invalid file path" << std::endl;
	}


	return 0;
}

