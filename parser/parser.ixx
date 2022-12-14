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

	std::vector<ast::stmt::StmtPtr> Parse();
private:
	ast::stmt::StmtPtr Declaration();
	ast::stmt::StmtPtr ClassDeclaration();
	ast::stmt::StmtPtr Statement();
	std::unique_ptr<ast::stmt::Function> Function(const std::string& kind);
	ast::stmt::StmtPtr ForStatement();
	ast::stmt::StmtPtr IfStatement();
	ast::stmt::StmtPtr VarDeclaration();
	ast::stmt::StmtPtr WhileStatement();
	ast::stmt::StmtPtr ExprStmt();
	std::vector<ast::stmt::StmtPtr> Block();
	ast::stmt::StmtPtr PrintStmt();
	ast::stmt::StmtPtr ReturnStmt();

	ast::expr::ExprPtr Expression();
	ast::expr::ExprPtr Assigment();
	ast::expr::ExprPtr Or();
	ast::expr::ExprPtr And();
	ast::expr::ExprPtr Equality();
	ast::expr::ExprPtr Comparison();
	ast::expr::ExprPtr Term();
	ast::expr::ExprPtr Factor();
	ast::expr::ExprPtr Unary();
	ast::expr::ExprPtr FinishCall(ast::expr::ExprPtr callee);
	ast::expr::ExprPtr Call();
	ast::expr::ExprPtr Primary();

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

std::vector<ast::stmt::StmtPtr> Parser::Parse() try
{
	std::vector<ast::stmt::StmtPtr> statements;
	while (!IsAtEnd())
		statements.push_back(Declaration());
	return statements;
}
catch (const ParseError& error)
{
	return {};
}

ast::stmt::StmtPtr Parser::Declaration() try
{
	if (Match(TokenType::CLASS))
		return ClassDeclaration();
	if (Match(TokenType::FUN))
		return Function("function");
	if (Match(TokenType::VAR))
		return VarDeclaration();
	return Statement();
}
catch (const ParseError& err)
{
	Synchronize();
	return nullptr;
}

ast::stmt::StmtPtr Parser::ClassDeclaration()
{
	auto name = ConsumeType(TokenType::IDENTIFIER, "Expect class name.");
	std::unique_ptr<ast::expr::Variable> superclass;
	if (Match(TokenType::LESS))
	{
		ConsumeType(TokenType::IDENTIFIER, "Expect superclass name.");
		superclass = std::make_unique<ast::expr::Variable>(Previous());
	}
	ConsumeType(TokenType::LEFT_BRACE, "Expect '{' before class body.");
	std::vector<std::unique_ptr<ast::stmt::Function>> methods;
	while (!CheckCurrentType(TokenType::RIGHT_BRACE) && !IsAtEnd())
	{
		methods.push_back(Function("method"));
	}
	ConsumeType(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
	return std::make_unique<ast::stmt::Class>(
		std::move(name), std::move(superclass), std::move(methods));
}

ast::stmt::StmtPtr Parser::Statement()
{
	if (Match(TokenType::FOR))
		return ForStatement();
	if (Match(TokenType::IF))
		return IfStatement();
	if (Match(TokenType::PRINT))
		return PrintStmt();
	if (Match(TokenType::RETURN))
		return ReturnStmt();
	if (Match(TokenType::WHILE))
		return WhileStatement();
	if (Match(TokenType::LEFT_BRACE))
		return std::make_unique<ast::stmt::Block>(Block());
	return ExprStmt();
}

std::unique_ptr<ast::stmt::Function> Parser::Function(const std::string& kind)
{
	auto name = ConsumeType(TokenType::IDENTIFIER, "Expect" + kind + " name.");
	ConsumeType(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
	std::vector<Token> parameters;
	if (!CheckCurrentType(TokenType::RIGHT_PAREN))
	{
		do
		{
			if (parameters.size() >= 255)
				Error(Peek(), "Can't have more than 255 parameters.");
			parameters.push_back(ConsumeType(TokenType::IDENTIFIER, "Expect parameter name."));
		} while (Match(TokenType::COMMA));
	}
	ConsumeType(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
	ConsumeType(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
	auto body = Block();
	return std::make_unique<ast::stmt::Function>
		(std::move(name), std::move(parameters), std::move(body));
}

ast::stmt::StmtPtr Parser::ForStatement()
{
	ConsumeType(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
	ast::stmt::StmtPtr initializer;
	if (Match(TokenType::SEMICOLON))
	{
	}
	else if (Match(TokenType::VAR))
		initializer = VarDeclaration();
	else
		initializer = ExprStmt();

	ast::expr::ExprPtr condition;
	if (!CheckCurrentType(TokenType::SEMICOLON))
		condition = Expression();
	ConsumeType(TokenType::SEMICOLON, "Expect ';' after loop condition.");

	ast::expr::ExprPtr increment;
	if (!CheckCurrentType(TokenType::LEFT_PAREN))
		increment = Expression();
	ConsumeType(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
	auto body = Statement();
	if (increment)
	{
		std::vector<ast::stmt::StmtPtr> block;
		block.push_back(std::move(body));
		block.push_back(std::make_unique<ast::stmt::Expression>(std::move(increment)));
		body = std::make_unique<ast::stmt::Block>(std::move(block));
	}
	if (!condition)
		condition = std::make_unique<ast::expr::Literal>(true);
	body = std::make_unique<ast::stmt::While>(std::move(condition), std::move(body));
	if (initializer)
	{
		std::vector<ast::stmt::StmtPtr> block;
		block.push_back(std::move(initializer));
		block.push_back(std::move(body));
		body = std::make_unique<ast::stmt::Block>(std::move(block));
	}
	return body;
}

ast::stmt::StmtPtr Parser::IfStatement()
{
	ConsumeType(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
	auto condition = Expression();
	ConsumeType(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
	auto then_branch = Statement();
	ast::stmt::StmtPtr else_branch;
	if (Match(TokenType::ELSE))
		else_branch = Statement();
	return std::make_unique<ast::stmt::If>(std::move(condition),
		std::move(then_branch), std::move(else_branch));
}

ast::stmt::StmtPtr Parser::VarDeclaration()
{
	auto name = ConsumeType(TokenType::IDENTIFIER, "Expect variable name.");
	ast::expr::ExprPtr initializer;
	if (Match(TokenType::EQUAL))
		initializer = Expression();
	ConsumeType(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
	return std::make_unique<ast::stmt::Var>(std::move(name), std::move(initializer));
}

ast::stmt::StmtPtr Parser::WhileStatement()
{
	ConsumeType(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
	auto condition = Expression();
	ConsumeType(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
	auto body = Statement();
	return std::make_unique<ast::stmt::While>(std::move(condition), std::move(body));
}

ast::stmt::StmtPtr Parser::ExprStmt()
{
	auto expr = Expression();
	ConsumeType(TokenType::SEMICOLON, "Expect ';' after expression.");
	return std::make_unique<ast::stmt::Expression>(std::move(expr));
}

std::vector<ast::stmt::StmtPtr> Parser::Block()
{
	std::vector<ast::stmt::StmtPtr> res;
	while (!CheckCurrentType(TokenType::RIGHT_BRACE) && !IsAtEnd())
		res.push_back(Declaration());
	ConsumeType(TokenType::RIGHT_BRACE, "Expect '}' after block.");
	return res;
}

ast::stmt::StmtPtr Parser::PrintStmt()
{
	auto expr = Expression();
	ConsumeType(TokenType::SEMICOLON, "Expect ';' after value.");
	return std::make_unique<ast::stmt::Print>(std::move(expr));
}

ast::stmt::StmtPtr Parser::ReturnStmt()
{
	auto keyword = Previous();
	ast::expr::ExprPtr val;
	if (!CheckCurrentType(TokenType::SEMICOLON))
		val = Expression();
	ConsumeType(TokenType::SEMICOLON, "Expect ';' after return value.");
	return std::make_unique<ast::stmt::Return>(std::move(keyword), std::move(val));
}

ast::expr::ExprPtr Parser::Expression()
{
	return Assigment();
}

ast::expr::ExprPtr Parser::Assigment()
{
	auto expr = Or();
	if (Match(TokenType::EQUAL))
	{
		auto equals = Previous();
		auto value = Assigment();
		const auto var = dynamic_cast<ast::expr::Variable*>(expr.get());
		const auto getter = dynamic_cast<ast::expr::Get*>(expr.get());
		if (var)
		{
			auto name = var->name;
			return std::make_unique<ast::expr::Assign>(std::move(name), std::move(value));
		}
		else if (getter)
		{
			return std::make_unique<ast::expr::Set>
				(std::move(getter->object), getter->name, std::move(value));
		}
		Error(equals, "Invalid assigment target.");
	}
	return expr;
}

ast::expr::ExprPtr Parser::Or()
{
	auto expr = And();
	while (Match(TokenType::OR))
	{
		auto op = Previous();
		auto right = And();
		expr = std::make_unique<ast::expr::Logical>(std::move(expr), std::move(op), std::move(right));

	}
	return expr;
}

ast::expr::ExprPtr Parser::And()
{
	auto expr = Equality();
	while (Match(TokenType::AND))
	{
		auto op = Previous();
		auto right = Equality();
		expr = std::make_unique<ast::expr::Logical>(std::move(expr), std::move(op), std::move(right));

	}
	return expr;
}

ast::expr::ExprPtr Parser::Equality()
{
	auto expr = Comparison();
	while (Match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL))
	{
		auto op = Previous();
		auto right = Comparison();
		expr = std::make_unique<ast::expr::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::expr::ExprPtr Parser::Comparison()
{
	auto expr = Term();
	while (Match(TokenType::GREATER, TokenType::GREATER_EQUAL,
		TokenType::LESS, TokenType::LESS_EQUAL))
	{
		auto op = Previous();
		auto right = Term();
		expr = std::make_unique<ast::expr::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::expr::ExprPtr Parser::Term()
{
	auto expr = Factor();
	while (Match(TokenType::MINUS, TokenType::PLUS))
	{
		auto op = Previous();
		auto right = Factor();
		expr = std::make_unique<ast::expr::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::expr::ExprPtr Parser::Factor()
{
	auto expr = Unary();
	while (Match(TokenType::SLASH, TokenType::STAR))
	{
		auto op = Previous();
		auto right = Unary();
		expr = std::make_unique<ast::expr::Binary>
			(std::move(expr), std::move(op), std::move(right));
	}
	return expr;
}

ast::expr::ExprPtr Parser::Unary()
{
	if (Match(TokenType::BANG, TokenType::MINUS))
	{
		auto op = Previous();
		auto right = Unary();
		return std::make_unique<ast::expr::Unary>
			(std::move(op), std::move(right));
	}
	return Call();
}

ast::expr::ExprPtr Parser::FinishCall(ast::expr::ExprPtr callee)
{
	std::vector<ast::expr::ExprPtr> arguments;
	if (!CheckCurrentType(TokenType::RIGHT_PAREN))
	{
		do
		{
			if (arguments.size() >= 255)
				Error(Peek(), "Can't have more than 255 arguments.");
			arguments.push_back(Expression());
		} while (Match(TokenType::COMMA));
	}
	auto paren = ConsumeType(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
	return std::make_unique<ast::expr::Call>
		(std::move(callee), std::move(paren), std::move(arguments));
}

ast::expr::ExprPtr Parser::Call()
{
	auto expr = Primary();
	while (true)
	{
		if (Match(TokenType::LEFT_PAREN))
			expr = FinishCall(std::move(expr));
		else if (Match(TokenType::DOT))
		{
			auto name = ConsumeType(TokenType::IDENTIFIER, "Expect property name after '.'.");
			expr = std::make_unique<ast::expr::Get>(std::move(expr), std::move(name));
		}
		else
			break;
	}
	return expr;
}

ast::expr::ExprPtr Parser::Primary()
{
	if (Match(TokenType::FALSE))
		return std::make_unique<ast::expr::Literal>(false);
	if (Match(TokenType::TRUE))
		return std::make_unique<ast::expr::Literal>(true);
	if (Match(TokenType::NIL))
		return std::make_unique<ast::expr::Literal>(std::monostate{});
	if (Match(TokenType::NUMBER, TokenType::STRING))
		return std::make_unique<ast::expr::Literal>(Previous().m_literal);
	if (Match(TokenType::SUPER))
	{
		auto keyword = Previous();
		ConsumeType(TokenType::DOT, "Expect '.' after 'super'.");
		auto method = ConsumeType(TokenType::IDENTIFIER, "Expect superclass method name.");
		return std::make_unique<ast::expr::Super>(std::move(keyword), std::move(method));
	}
	if (Match(TokenType::THIS))
		return std::make_unique<ast::expr::This>(Previous());
	if (Match(TokenType::IDENTIFIER))
		return std::make_unique<ast::expr::Variable>(Previous());
	if (Match(TokenType::LEFT_PAREN))
	{
		auto expr = Expression();
		ConsumeType(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
		return std::make_unique<ast::expr::Grouping>(std::move(expr));
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

