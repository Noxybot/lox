#include "Lexer.h"

Lexer::Lexer(std::string text)
	: m_text(std::move(text))
{
	m_text.erase(std::remove_if(m_text.begin(), m_text.end(), isspace), m_text.end());
}

Token Lexer::GetNextToken()
{
	if (m_curr_pos >= m_text.size())
		return Token(TokenType::EOFF, ' ');
	char ch = m_text[m_curr_pos];
	std::string curr_num_val;
	while (std::isdigit(ch))
	{
		curr_num_val += ch;
		++m_curr_pos;
		if (m_curr_pos >= m_text.size())
			break;
		ch = m_text[m_curr_pos];
	}
	if (!curr_num_val.empty())
	{
		return Token(TokenType::INTEGER, std::move(curr_num_val));
	}
	if (ch == '+')
	{
		++m_curr_pos;
		return Token(TokenType::PLUS, ch);
	}
	if (ch == '-')
	{
		++m_curr_pos;
		return Token(TokenType::MINUS, ch);
	}
	if (ch == '*')
	{
		++m_curr_pos;
		return Token(TokenType::MUL, ch);
	}
	if (ch == '/')
	{
		++m_curr_pos;
		return Token(TokenType::DIV, ch);
	}
	if (ch == '(')
	{
		++m_curr_pos;
		return Token(TokenType::LPARENT, ch);
	}
	if (ch == ')')
	{
		++m_curr_pos;
		return Token(TokenType::RPARENT, ch);
	}
	throw std::exception("Unknown token");
}
