#pragma once

#include "TokenTypes.h"

#include <string>

struct Token
{
public:
	std::string m_val;
	TokenType m_type = TokenType::UNDEF;

	Token() = default;
	Token(TokenType type, char val);
	Token(TokenType type, std::string val);

	//explicit operator std::string();
};