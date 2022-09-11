#include "Num.h"

namespace ast
{

Num::Num(const Token& token)
{
	if (token.m_type != TokenType::INTEGER)
		Error("Invalid type, can`t construct Num node");
	m_val = atoi(token.m_val.c_str());
}

int Num::Accept(Visitor& visitor) const
{
	return visitor.Visit(*this);
}

int Num::GetNum() const
{
	return m_val;
}
}
