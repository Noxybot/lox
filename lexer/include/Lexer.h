#pragma once

#include "Token.h"

#include <string>

class Lexer
{
	std::string m_text;
	int m_curr_pos = 0;
public:
	explicit Lexer(std::string text);
	Token GetNextToken();
};
