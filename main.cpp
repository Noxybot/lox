#include "Interpreter.h"
#include "Lexer.h"
#include "Parser.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) try
{
	

	while (true)
	{
		std::string text;
		std::getline(std::cin, text);
		
		auto l = std::make_shared<Lexer>(std::move(text));
		auto p = std::make_shared<Parser>(std::move(l));
		auto i = std::make_shared<Interpreter>(std::move(p));
		std::cout << i->Interpret() << std::endl;
	}
	return 0;
}
catch (const std::exception& exp)
{
	std::cerr << "Error occured: " << exp.what() << '\n';
	return -1;
}