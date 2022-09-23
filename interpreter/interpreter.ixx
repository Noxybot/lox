export module interpreter;

import ast;
import core;
import log;
import :environment;

import <functional>;
import <stdexcept>;
import <string>;
import <sstream>;
import <typeinfo>;
import <memory>;
import <map>;
import <variant>;
import <vector>;
import <iostream>;

class LoxFunction;
class Resolver;

namespace std
{
	template<>
	struct less<std::reference_wrapper<const ast::expr::Expr>>
	{
		bool operator() (const std::reference_wrapper<const ast::expr::Expr>& lhs, const std::reference_wrapper<const ast::expr::Expr>& rhs) const
		{
			return std::addressof(lhs.get()) < std::addressof(rhs.get());
		}
	};
}

export class Interpreter : public ast::expr::VisitorExpr, ast::stmt::VisitorStmt
{
	friend class LoxFunction;
	friend class Resolver;
	const std::shared_ptr<Environment> m_globals;
	std::shared_ptr<Environment> m_environment;
	std::map<std::reference_wrapper<const ast::expr::Expr>, int> m_locals;
public:
	Interpreter();
	void Interpret(const std::vector<ast::stmt::StmtPtr>& statements);
private:
	std::any Visit(const ast::stmt::Expression& val) override;
	std::any Visit(const ast::stmt::Function& val) override;
	std::any Visit(const ast::stmt::If& val) override;
	std::any Visit(const ast::stmt::Print& val) override;
	std::any Visit(const ast::stmt::Return& val) override;
	std::any Visit(const ast::stmt::Var& val) override;
	std::any Visit(const ast::stmt::While& val) override;
	std::any Visit(const ast::stmt::Block& val) override;
	std::any Visit(const ast::stmt::Class& val) override;

	std::any Visit(const ast::expr::Assign& val) override;
	std::any Visit(const ast::expr::Variable& val) override;
	std::any LookUpVariable(const Token& name, const ast::expr::Expr& expr);
	std::any Visit(const ast::expr::Binary& val) override;
	std::any Visit(const ast::expr::Call& val) override;
	std::any Visit(const ast::expr::Get& val) override;
	std::any Visit(const ast::expr::Grouping& val) override;
	std::any Visit(const ast::expr::Literal& val) override;
	std::any Visit(const ast::expr::Logical& val) override;
	std::any Visit(const ast::expr::Set& val) override;
	std::any Visit(const ast::expr::This& val) override;
	std::any Visit(const ast::expr::Unary& val) override;

	void Execute(const ast::stmt::Stmt& stmt);
	void Resolve(const ast::expr::Expr& expr, int depth);
	void ExecuteBlock(const std::vector<ast::stmt::StmtPtr>& statements,
		std::shared_ptr<Environment> environment);

	std::any Evaluate(const ast::expr::Expr& expr);
	static bool IsTruthy(const std::any& val);
	static bool IsEqual(const std::any& left, const std::any& right);
	static void CheckNumberOperand(const Token& op, const std::any& operand);
	static void CheckNumberOperands(const Token& op, const std::any& left, const std::any& right);
	static std::string Stringify(const std::any& val);
};

//TODO: move all this to interpreter.cpp
//For now it`s here because intellisense doesnt support syntax in module imp files

module :private;
