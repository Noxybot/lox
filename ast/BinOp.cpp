#include "BinOp.h"


namespace ast
{
BinOp::BinOp(NodePtr left, TokenType op_type, NodePtr right)
	: m_left(std::move(left))
	, m_right(std::move(right))
	, m_op_type(op_type)
{
	if (op_type != TokenType::DIV &&
		op_type != TokenType::MUL &&
		op_type != TokenType::PLUS &&
		op_type != TokenType::MINUS)
		Error("Can`t construct BinOp node");
}

int BinOp::Accept(Visitor& visitor) const
{
	return visitor.Visit(*this);
}

TokenType BinOp::GetOpType() const
{
	return m_op_type;
}

const Node& BinOp::GetLeftNode() const
{
	return *m_left;
}

const Node& BinOp::GetRightNode() const
{
	return *m_right;
}
}
