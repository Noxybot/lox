export module resolver;

import ast;
import core;
import log;
import interpreter;


import <vector>;
import <string>;
import <unordered_map>;

enum class FunctionType
{
	NONE,
	FUNCTION,
	INITIALIZER,
	METHOD,
};
enum class ClassType
{
	NONE,
	CLASS,
};

export class Resolver : ast::expr::VisitorExpr, ast::stmt::VisitorStmt
{
	Interpreter& m_interpreter;
	std::vector<std::unordered_map<std::string, bool>> m_scopes;
	FunctionType m_current_function_type = FunctionType::NONE;
	ClassType m_current_class_type = ClassType::NONE;

public:
	explicit Resolver(Interpreter& interpreter);
	void Resolve(const std::vector<ast::stmt::StmtPtr>& statements);
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
	std::any Visit(const ast::expr::Binary& val) override;
	std::any Visit(const ast::expr::Call& val) override;
	std::any Visit(const ast::expr::Get& val) override;
	std::any Visit(const ast::expr::Grouping& val) override;
	std::any Visit(const ast::expr::Literal& val) override;
	std::any Visit(const ast::expr::Logical& val) override;
	std::any Visit(const ast::expr::Set& val) override;
	std::any Visit(const ast::expr::This& val) override;
	std::any Visit(const ast::expr::Unary& val) override;

	void BeginScope();
	void EndScope();
	void Declare(const Token& name);
	void Define(const Token& name);
	void ResolveLocal(const ast::expr::Expr& expr, const Token& name);

	void Resolve(const ast::stmt::Stmt& statement);
	void Resolve(const ast::expr::Expr& expr);
	void ResolveFunction(const ast::stmt::Function& function, FunctionType type);
};


module :private;

Resolver::Resolver(Interpreter& interpreter)
	: m_interpreter(interpreter)
{}

std::any Resolver::Visit(const ast::stmt::Expression& val)
{
	Resolve(*val.expression);
	return {};
}

std::any Resolver::Visit(const ast::stmt::Function & val)
{
	//Declare(val.name);
	Define(val.name);
	ResolveFunction(val, FunctionType::FUNCTION);
	return {};
}

std::any Resolver::Visit(const ast::stmt::If& val)
{
	Resolve(*val.condition);
	Resolve(*val.then_branch);
	if (val.else_branch)
		Resolve(*val.else_branch);
	return {};
}

std::any Resolver::Visit(const ast::stmt::Print& val)
{
	Resolve(*val.expression);
	return {};
}

std::any Resolver::Visit(const ast::stmt::Return& val)
{
	if (m_current_function_type == FunctionType::NONE)
		Error(val.keyword, "Can't return form top-level code.");
	if (val.value)
	{
		if (m_current_function_type == FunctionType::INITIALIZER)
			Error(val.keyword, "Can't return a value from an initializer.");
		Resolve(*val.value);
	}
	return {};
}

std::any Resolver::Visit(const ast::stmt::Var& val)
{
	Declare(val.name);
	if (val.initializer)
		Resolve(*val.initializer);
	Define(val.name);
	return {};
}

std::any Resolver::Visit(const ast::stmt::While& val)
{
	Resolve(*val.condition);
	Resolve(*val.body);
	return {};
}

std::any Resolver::Visit(const ast::stmt::Block& val)
{
	BeginScope();
	Resolve(val.statements);
	EndScope();
	return {};
}

std::any Resolver::Visit(const ast::stmt::Class& val)
{
	auto enclosing_class = m_current_class_type;
	m_current_class_type = ClassType::CLASS;
	Define(val.name);
	BeginScope();
	m_scopes.back()["this"] = true;
	for (const auto& method : val.methods)
	{
		auto declaration = FunctionType::METHOD;
		if (method->name.m_lexeme == "init")
			declaration = FunctionType::INITIALIZER;
		ResolveFunction(*method, declaration);
	}
	EndScope();
	m_current_class_type = enclosing_class;
	return {};
}

std::any Resolver::Visit(const ast::expr::Assign& val)
{
	Resolve(*val.value);
	ResolveLocal(val, val.name);
	return {};
}

std::any Resolver::Visit(const ast::expr::Variable& val)
{
	if (!m_scopes.empty())
	{
		const auto& scope = m_scopes.back();
		const auto it = scope.find(val.name.m_lexeme);
		if (it != std::end(scope) && it->second == false)
		{
			Error(val.name, "Can't read local variable in its own initializer.");
		}
	}
	ResolveLocal(val, val.name);
	return {};
}

std::any Resolver::Visit(const ast::expr::Binary& val)
{
	Resolve(*val.right);
	Resolve(*val.left);
	return {};
}

std::any Resolver::Visit(const ast::expr::Call& val)
{
	Resolve(*val.callee);
	for (const auto& argument : val.arguments)
		Resolve(*argument);
	return {};
}

std::any Resolver::Visit(const ast::expr::Get& val)
{
	Resolve(*val.object);
	return {};
}

std::any Resolver::Visit(const ast::expr::Grouping& val)
{
	Resolve(*val.expression);
	return {};
}

std::any Resolver::Visit(const ast::expr::Literal& val)
{
	return {};
}

std::any Resolver::Visit(const ast::expr::Logical& val)
{
	Resolve(*val.left);
	Resolve(*val.right);
	return {};
}

std::any Resolver::Visit(const ast::expr::Set& val)
{
	Resolve(*val.value);
	Resolve(*val.object);
	return {};
}

std::any Resolver::Visit(const ast::expr::This& val)
{
	if (m_current_class_type == ClassType::NONE)
	{
		Error(val.keyword, "Can't use 'this' outside of a class.");
		return {};
	}
	ResolveLocal(val, val.keyword);
	return {};
}

std::any Resolver::Visit(const ast::expr::Unary& val)
{
	Resolve(*val.right);
	return {};
}

void Resolver::BeginScope()
{
	m_scopes.push_back({});
}

void Resolver::EndScope()
{
	m_scopes.pop_back();
}

void Resolver::Declare(const Token& name)
{
	if (m_scopes.empty())
		return;
	auto& scope = m_scopes.back();
	const auto emplace_res = scope.emplace(name.m_lexeme, false);
	if (!emplace_res.second)
		Error(name, "Already a variable with this name in this scope.");
}

void Resolver::Define(const Token& name)
{
	if (m_scopes.empty())
		return;
	m_scopes.back()[name.m_lexeme] = true;
}

void Resolver::ResolveLocal(const ast::expr::Expr& expr, const Token& name)
{
	for (int i = m_scopes.size() - 1; i >= 0; --i)
	{
		const auto& scope = m_scopes[i];
		if (scope.find(name.m_lexeme) != std::end(scope))
		{
			m_interpreter.Resolve(expr, m_scopes.size() - 1 - i);
			return;
		}
	}
}

void Resolver::Resolve(const std::vector<ast::stmt::StmtPtr>& statements)
{
	for (const auto& statement : statements)
		Resolve(*statement);
}

void Resolver::Resolve(const ast::stmt::Stmt& statement)
{
	statement.Accept(*this);
}

void Resolver::Resolve(const ast::expr::Expr& expr)
{
	expr.Accept(*this);
}

void Resolver::ResolveFunction(const ast::stmt::Function& function, FunctionType type)
{
	auto enclosing_function = m_current_function_type;
	m_current_function_type = type;
	BeginScope();
	for (const auto& param : function.params)
	{
		//Declare(param);
		Define(param);
	}
	Resolve(function.body);
	EndScope();
	m_current_function_type = enclosing_function;
}
