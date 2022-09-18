export module parser;

import ast;
import core;
import log;

import <memory>;
import <vector>;
import <stdexcept>;
import <string_view>;

export class ParseError : public std::runtime_error
{
public:
	using std::runtime_error::runtime_error;
};

export class Parser
{
	std::vector<Token> m_tokens;
	int m_current = 0;
public:
	explicit Parser(std::vector<Token> tokens);

	ast::ExprPtr Parse();
private:
	ast::ExprPtr Expression();
	ast::ExprPtr Equality();
	ast::ExprPtr Comparison();
	ast::ExprPtr Term();
	ast::ExprPtr Factor();
	ast::ExprPtr Unary();
	ast::ExprPtr Primary();

	const Token& Advance();
	bool IsAtEnd() const;
	const Token& Peek() const;
	const Token& Previous() const;
	const Token& ConsumeType(TokenType type, std::string_view error);

	bool CheckCurrentType(TokenType type) const;
	ParseError Error(const Token& token, std::string_view message);
	void Synchronize();
	
	template<class ...TokenType>
	bool Match(TokenType... args)
	{
		if ((CheckCurrentType(args) || ...))
		{
			Advance();
			return true;
		}
		return false;
	}
};

module :private;

Parser::Parser(std::vector<Token> tokens)
	: m_tokens(std::move(tokens))
{}

ast::ExprPtr Parser::Parse() try
{
	return Expression();
}
catch (const ParseError& error)
{
	return nullptr;
}

ast::ExprPtr Parser::Expression()
{
	return Equality();
}

ast::ExprPtr Parser::Equality()
{
	auto expr = Comparison();
	while (Match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
	{
		auto op = Previous();
		auto right = Comparison();
		expr = std::make_unique<ast::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::ExprPtr Parser::Comparison()
{
	auto expr = Term();
	while (Match(TokenType::GREATER, TokenType::GREATER_EQUAL,
		TokenType::LESS, TokenType::LESS_EQUAL))
	{
		auto op = Previous();
		auto right = Term();
		expr = std::make_unique<ast::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::ExprPtr Parser::Term()
{
	auto expr = Factor();
	while (Match(TokenType::MINUS, TokenType::PLUS))
	{
		auto op = Previous();
		auto right = Factor();
		expr = std::make_unique<ast::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::ExprPtr Parser::Factor()
{
	auto expr = Unary();
	while (Match(TokenType::SLASH, TokenType::STAR))
	{
		auto op = Previous();
		auto right = Unary();
		expr = std::make_unique<ast::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::ExprPtr Parser::Unary()
{
	if (Match(TokenType::BANG, TokenType::MINUS))
	{
		auto op = Previous();
		auto right = Unary();
		return std::make_unique<ast::Unary>
			(std::move(op), std::move(right));
	}
	return Primary();
}

ast::ExprPtr Parser::Primary()
{
	if (Match(TokenType::FALSE))
		return std::make_unique<ast::Literal>(false);
	if (Match(TokenType::TRUE))
		return std::make_unique<ast::Literal>(true);
	if (Match(TokenType::NIL))
		return std::make_unique<ast::Literal>(std::monostate{});
	if (Match(TokenType::NUMBER, TokenType::STRING))
		return std::make_unique<ast::Literal>(Previous().m_literal);
	if (Match(TokenType::LEFT_PAREN))
	{
		auto expr = Expression();
		ConsumeType(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
		return std::make_unique<ast::Grouping>(std::move(expr));
	}
	throw Error(Peek(), "Expect expression.");
}

const Token& Parser::Advance()
{
	if (!IsAtEnd())
		++m_current;
	return Previous();
}

bool Parser::IsAtEnd() const
{
	return Peek().m_type == TokenType::END_OF_FILE;
}

const Token& Parser::Peek() const
{
	return m_tokens[m_current];
}

const Token& Parser::Previous() const
{
	return m_tokens[m_current - 1];
}

const Token& Parser::ConsumeType(TokenType type, std::string_view error)
{
	if (CheckCurrentType(type))
		return Advance();
	throw Error(Peek(), error);
}

bool Parser::CheckCurrentType(TokenType type) const
{
	if (IsAtEnd())
		return false;
	return Peek().m_type == type;
}

ParseError Parser::Error(const Token& token, std::string_view message)
{
	::Error(token, message);
	return ParseError{"Parsing error"};
}

void Parser::Synchronize()
{
	Advance();
	while (!IsAtEnd())
	{
		if (Previous().m_type == TokenType::SEMICOLON)
			return;
		switch (Peek().m_type)
		{
			case TokenType::CLASS:
			case TokenType::FUN:
			case TokenType::VAR:
			case TokenType::FOR:
			case TokenType::IF:
			case TokenType::WHILE:
			case TokenType::PRINT:
			case TokenType::RETURN:
				return;
		}
		Advance();
	}
}

