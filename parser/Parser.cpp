#include "Parser.h"

/*
Parser::Parser(std::shared_ptr<Lexer> lexer)
	: m_lexer(std::move(lexer))
{
	//m_curr_token = m_lexer->GetNextToken();
}

ast::NodePtr Parser::Factor()
{
	/*auto token = m_curr_token;
	if (token.m_type == TokenType::INTEGER)
	{
		Eat(TokenType::INTEGER);
		return std::make_shared<ast::Num>(token);
	}
	if (token.m_type == TokenType::LPARENT)
	{
		Eat(TokenType::LPARENT);
		auto res = Expr();
		Eat(TokenType::RPARENT);
		return res;
	}
	if (token.m_type == TokenType::PLUS || token.m_type == TokenType::MINUS)
	{
		Eat(token.m_type);
		return std::make_shared<ast::UnaryOp>(token.m_type, Factor());
	}
	return nullptr;#1#
	return 0;
}

ast::NodePtr Parser::Term()
{
	/*auto left = Factor();
	while (m_curr_token.m_type == TokenType::MUL || m_curr_token.m_type == TokenType::DIV)
	{
		auto type = m_curr_token.m_type;
		Eat(m_curr_token.m_type);
		switch (type)
		{
		case TokenType::MUL:
		case TokenType::DIV:
			left = std::make_shared<ast::BinOp>(std::move(left), type, Factor());
			break;
		default:
			Error("Error in term rule");
		}
	}
	return left;#1#
	return 0;
}

void Parser::Eat(TokenType type)
{
	/*if (m_curr_token.m_type == type)
		m_curr_token = m_lexer->GetNextToken();
	else
		throw std::exception("Syntax error: Unexpected token");#1#
}

//Parser+Interpreter

ast::NodePtr Parser::Expr()
{
	auto left = Term();
	while (m_curr_token.m_type == TokenType::PLUS || m_curr_token.m_type == TokenType::MINUS)
	{
		auto type = m_curr_token.m_type;
		Eat(m_curr_token.m_type);
		switch (type)
		{
		case TokenType::PLUS:
		case TokenType::MINUS:
			left = std::make_shared<ast::BinOp>(std::move(left), type, Term());
			break;
		default:
			Error("Error in term rule");
		}
	}
	return left;
}


ast::NodePtr Parser::Parse()
{
	return Expr();
}

void Parser::Error(const char* error)
{
	throw std::exception(error);
}
*/
