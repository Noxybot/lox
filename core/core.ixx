export module core;

import <ostream>;
import <variant>;
import <string>;
import <cassert>;

export using LiteralT = std::variant<std::monostate, std::string, double, bool>;

export enum class TokenType
{
	UNDEF,
	// Single-character tokens.
	LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
	COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

	// One or two character tokens.
	BANG, BANG_EQUAL,
	EQUAL, EQUAL_EQUAL,
	GREATER, GREATER_EQUAL,
	LESS, LESS_EQUAL,

	// Literals.
	IDENTIFIER, STRING, NUMBER,

	// Keywords.
	AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
	PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,

	END_OF_FILE,
};

std::string ToString(const TokenType type);

export struct AnyGet {
	std::string operator()(bool value) { return value ? "true" : "false"; }
	std::string operator()(char value) { return std::string(1, value); }
	std::string operator()(int value) { return std::to_string(value); }
	std::string operator()(double value) { return std::to_string(value); }
	std::string operator()(const std::string& value) { return value; }
	std::string operator()(const std::monostate& value) { return "NIL"; }
};

export struct Token
{
	TokenType m_type = TokenType::UNDEF;
	std::string m_lexeme;
	LiteralT m_literal;
	int m_line = -1;

	Token(TokenType type, std::string lexeme, LiteralT literal, int line)
		: m_type(type)
		, m_lexeme(std::move(lexeme))
		, m_literal(std::move(literal))
		, m_line(line)
	{}
	
	std::string ToString() const
	{
		std::string res;
		res += ::ToString(m_type);
		res += " ";
		res += m_lexeme;
		res += " ";
		res += std::visit(AnyGet{}, m_literal);
		return res;
	}
};


std::string ToString(const TokenType type)
{
	switch (type) {
	case TokenType::UNDEF: return "UNDEF";
	case TokenType::LEFT_PAREN: return "LEFT_PAREN";
	case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
	case TokenType::LEFT_BRACE: return "LEFT_BRACE";
	case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
	case TokenType::COMMA: return "COMMA";
	case TokenType::DOT: return "DOT";
	case TokenType::MINUS: return "MINUS";
	case TokenType::PLUS: return "PLUS";
	case TokenType::SEMICOLON: return "SEMICOLON";
	case TokenType::SLASH: return "SLASH";
	case TokenType::STAR: return "STAR";
	case TokenType::BANG: return "BANG";
	case TokenType::BANG_EQUAL: return "BANG_EQUAL";
	case TokenType::EQUAL: return "EQUAL";
	case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
	case TokenType::GREATER: return "GREATER";
	case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
	case TokenType::LESS: return "LESS";
	case TokenType::LESS_EQUAL: return "LESS_EQUAL";
	case TokenType::IDENTIFIER: return "IDENTIFIER";
	case TokenType::STRING: return "STRING";
	case TokenType::NUMBER: return "NUMBER";
	case TokenType::AND: return "AND";
	case TokenType::CLASS: return "CLASS";
	case TokenType::ELSE: return "ELSE";
	case TokenType::FALSE: return "FALSE";
	case TokenType::FUN: return "FUN";
	case TokenType::FOR: return "FOR";
	case TokenType::IF: return "IF";
	case TokenType::NIL: return "NIL";
	case TokenType::OR: return "OR";
	case TokenType::PRINT: return "PRINT";
	case TokenType::RETURN: return "RETURN";
	case TokenType::SUPER: return "SUPER";
	case TokenType::THIS: return "THIS";
	case TokenType::TRUE: return "TRUE";
	case TokenType::VAR: return "VAR";
	case TokenType::WHILE: return "WHILE";
	case TokenType::END_OF_FILE: return "END_OF_FILE";
	default:
		assert(false);
		return std::to_string(static_cast<int>(type));
	}
}
