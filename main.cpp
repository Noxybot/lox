import lexer;

#include <iostream>
#include <fstream>
#include <exception>

int main(int argc, char** argv) try
{
	if (argc > 2)
	{
		std::cerr << "Usage: ./clox [script]\n";
		return 64;
	}
	
	if (argc == 2)
	{
		std::ifstream f{ argv[1] };
		std::string text((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());
		Lexer l{ std::move(text) };
		//const auto i = Interpreter::Create(std::move(text));
		//i->Interpret();
	}
	else
	{
		
	}

	while (true)
	{
		std::cout << "> ";
		std::string text;
		std::getline(std::cin, text);
		if (text.empty())
			break;
		Lexer l{ std::move(text) };
		auto tokens = l.GetTokens();
		for (const auto& token : tokens)
		{
			std::cout << token.ToString() << "\n";
		}
		//const auto i = Interpreter::Create(std::move(text));
		//i->Interpret();
	}
	return 0;
}
catch (const std::exception& exp)
{
	std::cerr << "Error occurred: " << exp.what() << '\n';
	return -1;
}
