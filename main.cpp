#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

import lexer;
import parser;
import interpreter;
import log;
import ast_printer;

void Run(std::string source) noexcept(false)
{
	Lexer l{ std::move(source) };
	auto tokens = l.GetTokens();
	Parser p{ std::move(tokens) };
	auto expr = p.Parse();
	if (has_error)
		return;
	static Interpreter i;
	i.Interpret(*expr);
	//ASTPrinter printer;
	//std::cout << printer.Print(*expr) << std::endl;

}

void RunFile(std::string_view path) noexcept(false)
{
	std::ifstream f{ std::filesystem::path(path) };
	std::string text((std::istreambuf_iterator<char>(f)),
		std::istreambuf_iterator<char>());
	Run(std::move(text));
}

void RunPrompt() noexcept(false)
{
	while (true)
	{
		std::cout << "> ";
		std::string text;
		std::getline(std::cin, text);
		if (text.empty())
			return;
		Run(std::move(text));
		has_error = false;
	}
}

int main(int argc, char** argv) try
{
	if (argc > 2)
	{
		std::cerr << "Usage: ./clox [script]\n";
		return 64;
	}
	
	if (argc == 2)
	{
		RunFile(argv[1]);
	}
	else
	{
		RunPrompt();
	}
	if (has_error)
		return 65;
	if (has_runtime_error)
		return 70;
	return 0;
}
catch (const std::exception& error)
{
	std::cerr << error.what() << std::endl;
	return -1;
}