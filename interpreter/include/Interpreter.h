#pragma once

#include <memory>
#include <string>

import ast;


class Parser;

class Interpreter //: public ast::Visitor
{
	std::shared_ptr<Parser> m_parser;
public:
	int Interpret();
	static std::shared_ptr<Interpreter> Create(std::string text);
private:
	explicit Interpreter(std::shared_ptr<Parser> parser);
	struct make_shared_enabler;
	//std::any Visit(const ast::Num& num_node) override;
	//std::any Visit(const ast::BinOp& bin_op_node) override;
	//std::any Visit(const ast::UnaryOp& unary_op_node) override;
};