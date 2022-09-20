export module ast_printer;

import <any>;
import <string>;

import ast;

export class ASTPrinter : public ast::expr::VisitorExpr
{
	std::any Visit(const ast::expr::Binary& val) override;
	std::any Visit(const ast::expr::Grouping& val) override;
	std::any Visit(const ast::expr::Literal& val) override;
	std::any Visit(const ast::expr::Unary& val) override;

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
	std::string Print(const ast::expr::Expr& root);
};

module :private;

std::any ASTPrinter::Visit(const ast::expr::Binary& val)
{
	return Parenthesize(val.op.m_lexeme, *val.left, *val.right);
}
std::any ASTPrinter::Visit(const ast::expr::Grouping& val)
{
	return Parenthesize("group", *val.expression);
}

std::any ASTPrinter::Visit(const ast::expr::Literal& val)
{
	return std::visit(AnyGet{}, val.value);
}

std::any ASTPrinter::Visit(const ast::expr::Unary& val)
{
	return Parenthesize(val.op.m_lexeme, *val.right);
}

std::string ASTPrinter::Print(const ast::expr::Expr& root)
{
	return std::any_cast<std::string>(root.Accept(*this));
}
