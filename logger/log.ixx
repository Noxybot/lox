export module log;

import core;

import <iostream>;
import <string_view>;
import <string>;

export bool has_error = false;

export void Report(int line, std::string_view where, std::string_view message);

export void Error(int line, std::string_view message);

export void Error(const Token& token, std::string_view message);

module :private;

void Report(int line, std::string_view where, std::string_view message)
{
	std::cerr << "[line " << std::to_string(line) <<
		" ] Error" << where << ": " << message << '\n';
	has_error = true;
}

void Error(int line, std::string_view message)
{
	Report(line, "", message);
}

void Error(const Token& token, std::string_view message)
{
	if (token.m_type == TokenType::END_OF_FILE)
		Report(token.m_line, " at end", message);
	else
		Report(token.m_line, " at '" + token.m_lexeme + "'", message);
}