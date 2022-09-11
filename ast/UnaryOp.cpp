#include "UnaryOp.h"

ast::UnaryOp::UnaryOp(TokenType op_type, NodePtr node)
	: m_op_type(op_type)
	, m_child(std::move(node))
{
	if (op_type != TokenType::MINUS && op_type != TokenType::PLUS)
		Error("Can`t construct UnaryOp");
}

int ast::UnaryOp::Accept(Visitor& visitor) const
{
	return visitor.Visit(*this);
}

TokenType ast::UnaryOp::GetOpType() const
{
	return m_op_type;
}

const ast::Node& ast::UnaryOp::GetChild() const
{
	return *m_child;
}
