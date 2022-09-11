#pragma once

#include "Parser.h"

#include <memory>

class Interpreter : public ast::Visitor
{
	std::shared_ptr<Parser> m_parser;
public:
	explicit Interpreter(std::shared_ptr<Parser> parser);
	int Interpret();
private:
	int Visit(const ast::Num& num_node) override;
	int Visit(const ast::BinOp& bin_op_node) override;
	int Visit(const ast::UnaryOp& unary_op_node) override;
};