#pragma once

#include "Visitor.h"

#include <memory>
#include <string>

namespace ast
{
	class Num; class BinOp; class UnaryOp;
}

class Parser;

class Interpreter : public ast::Visitor
{
	std::shared_ptr<Parser> m_parser;
public:
	int Interpret();
	static std::shared_ptr<Interpreter> Create(std::string text);
private:
	explicit Interpreter(std::shared_ptr<Parser> parser);
	struct make_shared_enabler;
	int Visit(const ast::Num& num_node) override;
	int Visit(const ast::BinOp& bin_op_node) override;
	int Visit(const ast::UnaryOp& unary_op_node) override;
};