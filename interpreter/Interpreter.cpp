#include "Interpreter.h"

#include "BinOp.h"
#include "Num.h"

#include <string>

#include "UnaryOp.h"

Interpreter::Interpreter(std::shared_ptr<Parser> parser)
	: m_parser(std::move(parser))
{}

int Interpreter::Interpret()
{
	auto root = m_parser->Parse();
	return root ? root->Accept(*this) : -1;
}

int Interpreter::Visit(const ast::Num& num_node)
{
	return num_node.GetNum();
}

int Interpreter::Visit(const ast::BinOp& bin_op_node)
{
	switch (bin_op_node.GetOpType())
	{
	case TokenType::PLUS:
		return bin_op_node.GetLeftNode().Accept(*this) + 
			bin_op_node.GetRightNode().Accept(*this);
	case TokenType::MINUS:
		return bin_op_node.GetLeftNode().Accept(*this) -
			bin_op_node.GetRightNode().Accept(*this);
	case TokenType::MUL:
		return bin_op_node.GetLeftNode().Accept(*this) *
			bin_op_node.GetRightNode().Accept(*this);
	case TokenType::DIV:
		return bin_op_node.GetLeftNode().Accept(*this) /
			bin_op_node.GetRightNode().Accept(*this);
	default:
		throw std::exception("Failed to interpret");
	}
}

int Interpreter::Visit(const ast::UnaryOp& unary_op_node)
{
	if (unary_op_node.GetOpType() == TokenType::MINUS)
		return -unary_op_node.GetChild().Accept(*this);
	return unary_op_node.GetChild().Accept(*this);
}
