#pragma once

#include "Token.h"
#include "Node.h"

#include <memory>

class Lexer;

class Parser
{
	Token m_curr_token;
	std::shared_ptr<Lexer> m_lexer;
public:
	explicit Parser(std::shared_ptr<Lexer> lexer);
	ast::NodePtr Factor();
	ast::NodePtr Term();
	void Eat(TokenType type);
	ast::NodePtr Expr();
	ast::NodePtr Parse();

private:
	void Error(const char* error);
};