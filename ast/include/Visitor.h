#pragma once

namespace ast
{
class Num;
class BinOp;
class UnaryOp;

class Visitor
{
public:
	virtual ~Visitor() = default;
	virtual int Visit(const Num& num_node) = 0;
	virtual int Visit(const BinOp& bin_op_node) = 0;
	virtual int Visit(const UnaryOp& unary_op_node) = 0;
};

}