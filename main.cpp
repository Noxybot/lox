#include "Interpreter.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) try
{
	while (true)
	{
		std::string text;
		std::getline(std::cin, text);
		
		const auto i = Interpreter::Create(std::move(text));
		std::cout << i->Interpret() << std::endl;
	}
	return 0;
}
catch (const std::exception& exp)
{
	std::cerr << "Error occurred: " << exp.what() << '\n';
	return -1;
}