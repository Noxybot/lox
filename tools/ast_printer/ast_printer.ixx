export module ast_printer;

import <any>;
import <string>;

import ast;

export class ASTPrinter : public ast::Visitor
{
	std::any Visit(const ast::Binary& val) override;
	std::any Visit(const ast::Grouping& val) override;
	std::any Visit(const ast::Literal& val) override;
	std::any Visit(const ast::Unary& val) override;

	template<class ...Expr>
	std::string Parenthesize(std::string_view name, const Expr&... expr)
	{
		std::string res;
		res += "(";
		res += name;
		(res += ... += (" " + std::any_cast<std::string>(expr.Accept(*this))));
		res += ")";
		return res;
	}

public:
	std::string Print(const ast::Expr& root);
};

module :private;

std::any ASTPrinter::Visit(const ast::Binary& val)
{
	return Parenthesize(val.op.m_lexeme, *val.left, *val.right);
}
std::any ASTPrinter::Visit(const ast::Grouping& val)
{
	return Parenthesize("group", *val.expression);
}

std::any ASTPrinter::Visit(const ast::Literal& val)
{
	return std::visit(AnyGet{}, val.value);
}

std::any ASTPrinter::Visit(const ast::Unary& val)
{
	return Parenthesize(val.op.m_lexeme, *val.right);
}

std::string ASTPrinter::Print(const ast::Expr& root)
{
	return std::any_cast<std::string>(root.Accept(*this));
}
