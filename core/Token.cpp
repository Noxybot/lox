#include "Token.h"

#include "Lexer.h"

Token::Token(TokenType type, char val)
	: m_val(1, val)
	, m_type(type)
{}

Token::Token(TokenType type, std::string val)
	: m_type(type)
	, m_val(std::move(val))
{}
