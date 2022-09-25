module;

#include "scope_exit.h"

module interpreter;

import ast;
import core;
import log;
//import utils;
import :loxclass;
import :loxcallable;

import <any>;
import <stdexcept>;
import <string>;
import <sstream>;
import <typeinfo>;
import <memory>;
import <variant>;
import <vector>;
import <iostream>;

template<class T>
bool CheckAnyType(const std::any& val)
{
	return typeid(T) == val.type();
}

template<>
bool CheckAnyType<std::string>(const std::any& val)
{
	return CheckAnyType<std::reference_wrapper<const std::string>>(val) || typeid(std::string) == val.type()
		|| CheckAnyType<std::reference_wrapper<std::string>>(val);
}

const std::string& GetStringRef(const std::any& str)
{
	if (typeid(std::reference_wrapper<const std::string>) == str.type())
		return std::any_cast<std::reference_wrapper<const std::string>>(str);
	if (typeid(std::string) == str.type())
		return std::any_cast<const std::string&>(str);
	return std::any_cast<std::reference_wrapper<std::string>>(str);
}

std::any Interpreter::Visit(const ast::stmt::Expression& val)
{
	Evaluate(*val.expression);
	return {};
}

std::any Interpreter::Visit(const ast::stmt::Function& val)
{
	auto function = std::static_pointer_cast<LoxCallable>(std::make_shared<LoxFunction>(val, m_environment));
	m_environment->Define(val.name.m_lexeme, function);
	return {};
}

std::any Interpreter::Visit(const ast::stmt::If& val)
{
	if (IsTruthy(Evaluate(*val.condition)))
		Execute(*val.then_branch);
	else if (val.else_branch)
		Execute(*val.else_branch);
	return {};
}

std::any Interpreter::Visit(const ast::stmt::Print& val)
{
	auto res = Evaluate(*val.expression);
	std::cout << Stringify(res) << std::endl;
	return {};
}
std::any Interpreter::Visit(const ast::stmt::Return& val)
{
	std::any ret_val;
	if (val.value)
		ret_val = Evaluate(*val.value);
	throw Return(std::move(ret_val));
}


std::any Interpreter::Visit(const ast::stmt::Var& val)
{
	std::any value;
	if (val.initializer)
		value = Evaluate(*val.initializer);
	m_environment->Define(val.name.m_lexeme, std::move(value));
	return {};
}

std::any Interpreter::Visit(const ast::stmt::While& val)
{
	while (IsTruthy(Evaluate(*val.condition)))
		Execute(*val.body);
	return {};
}

std::any Interpreter::Visit(const ast::stmt::Block& val)
{
	ExecuteBlock(val.statements, std::make_shared<Environment>(m_environment));
	return {};
}

std::any Interpreter::Visit(const ast::stmt::Class& val)
{
	std::shared_ptr<LoxClass> superclass;
	if (val.superclass)
	{
		auto sup = Evaluate(*val.superclass);
		if (!CheckAnyType<std::shared_ptr<LoxCallable>>(sup))
		{
			throw RuntimeError(val.superclass->name, "Superclass must be a class.");
		}
		superclass = std::dynamic_pointer_cast<LoxClass>(std::any_cast<std::shared_ptr<LoxCallable>>(sup));
		if (!superclass)
			throw RuntimeError(val.superclass->name, "Superclass must be a class.");
	}

	m_environment->Define(val.name.m_lexeme, std::any{});

	if (val.superclass)
	{
		m_environment = std::make_shared<Environment>(std::move(m_environment));
		m_environment->Define("super", superclass);
	}

	std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
	for (const auto& method : val.methods)
	{
		auto function = std::make_shared<LoxFunction>(
			*method, m_environment, method->name.m_lexeme == "init");
		methods[method->name.m_lexeme] = std::move(function);
	}

	auto klass = static_pointer_cast<LoxCallable>(
		std::make_shared<LoxClass>(val.name.m_lexeme,
			std::move(superclass),
			std::move(methods)));
	if (val.superclass)
		m_environment = m_environment->GetEnclosing();
	m_environment->Assign(val.name, std::move(klass));
	return {};
}

std::any Interpreter::Visit(const ast::expr::Assign& val)
{
	auto value = Evaluate(*val.value);
	const auto distance_it = m_locals.find(val);
	if (distance_it != std::end(m_locals))
		m_environment->AssignAt(distance_it->second, val.name, std::move(value));
	else
		m_globals->Assign(val.name, std::move(value));
	return value;
}

std::any Interpreter::Visit(const ast::expr::Variable& val)
{
	return LookUpVariable(val.name, val);
}

std::any Interpreter::LookUpVariable(const Token& name, const ast::expr::Expr& expr)
{
	const auto distance_it = m_locals.find(expr);
	if (distance_it != std::end(m_locals))
		return m_environment->GetAt(distance_it->second, name.m_lexeme);
	return m_globals->Get(name);
}

std::any Interpreter::Visit(const ast::expr::Binary& val)
{
	auto left = Evaluate(*val.left);
	auto right = Evaluate(*val.right);
	switch (val.op.m_type)
	{
	case TokenType::MINUS:
		CheckNumberOperands(val.op, left, right);
		return std::any_cast<double>(left) - std::any_cast<double>(right);
	case TokenType::SLASH:
		CheckNumberOperands(val.op, left, right);
		return std::any_cast<double>(left) / std::any_cast<double>(right);
	case TokenType::STAR:
		CheckNumberOperands(val.op, left, right);
		return std::any_cast<double>(left) * std::any_cast<double>(right);
	case TokenType::PLUS:
		if (CheckAnyType<double>(left) &&
			CheckAnyType<double>(right))
		{
			return std::any_cast<double>(left) + std::any_cast<double>(right);
		}
		if (CheckAnyType<std::string>(left) &&
			CheckAnyType<std::string>(right))
		{
			return GetStringRef(left) + GetStringRef(right);
		}
		if (CheckAnyType<std::string>(left) &&
			CheckAnyType<double>(right))
		{
			return GetStringRef(left) + std::to_string(std::any_cast<double>(right));
		}
		if (CheckAnyType<double>(left) &&
			CheckAnyType<std::string>(right))
		{
			return std::to_string(std::any_cast<double>(left)) + GetStringRef(right);
		}
		throw RuntimeError(val.op, "Operands must be two numbers or two strings.");
		break;
	case TokenType::GREATER:
		CheckNumberOperands(val.op, left, right);
		return std::any_cast<double>(left) > std::any_cast<double>(right);
	case TokenType::GREATER_EQUAL:
		CheckNumberOperands(val.op, left, right);
		return std::any_cast<double>(left) >= std::any_cast<double>(right);
	case TokenType::LESS:
		CheckNumberOperands(val.op, left, right);
		return std::any_cast<double>(left) < std::any_cast<double>(right);
	case TokenType::LESS_EQUAL:
		CheckNumberOperands(val.op, left, right);
		return std::any_cast<double>(left) <= std::any_cast<double>(right);
	case TokenType::BANG_EQUAL:
		return !IsEqual(left, right);
	case TokenType::EQUAL_EQUAL:
		return IsEqual(left, right);
	}
	return {};
}

std::any Interpreter::Visit(const ast::expr::Call& val)
{
	auto callee = Evaluate(*val.callee);
	std::vector<std::any> arguments;
	for (const auto& argument : val.arguments)
		arguments.push_back(Evaluate(*argument));
	if (!CheckAnyType<std::shared_ptr<LoxCallable>>(callee))
		throw RuntimeError(val.paren, "Can only call functions and classes.");
	auto function = std::any_cast<std::shared_ptr<LoxCallable>>(callee);
	if (arguments.size() != function->Arity())
	{
		throw RuntimeError(val.paren, "Expected " + std::to_string(function->Arity()) +
			" arguments but got " + std::to_string(arguments.size()) + ".");
	}
	return function->Call(*this, arguments);
}

std::any Interpreter::Visit(const ast::expr::Get& val)
{
	auto object = Evaluate(*val.object);
	if (CheckAnyType<std::shared_ptr<LoxInstance>>(object))
	{
		return std::any_cast<std::shared_ptr<LoxInstance>>(object)->Get(val.name);
	}
	throw RuntimeError(val.name, "Only instances have properties.");
}


std::any Interpreter::Visit(const ast::expr::Grouping& val)
{
	return Evaluate(*val.expression);
}

std::any Interpreter::Visit(const ast::expr::Literal& val)
{
	if (std::holds_alternative<std::monostate>(val.value))
		return {};
	if (std::holds_alternative<std::string>(val.value))
		return std::get<std::string>(val.value);
	if (std::holds_alternative<double>(val.value))
		return std::get<double>(val.value);
	if (std::holds_alternative<bool>(val.value))
		return std::get<bool>(val.value);
	//should be unreacable
	throw std::runtime_error("Unknown literal type.");
}

std::any Interpreter::Visit(const ast::expr::Logical& val)
{
	auto left = Evaluate(*val.left);
	if (val.op.m_type == TokenType::OR)
	{
		if (IsTruthy(left))
			return left;
	}
	else if (!IsTruthy(left))
		return left;
	return Evaluate(*val.right);
}

std::any Interpreter::Visit(const ast::expr::Set& val)
{
	auto object = Evaluate(*val.object);
	if (!CheckAnyType<std::shared_ptr<LoxInstance>>(object))
	{
		throw RuntimeError(val.name, "Only instances have fields.");
	}
	auto value = Evaluate(*val.value);
	std::any_cast<std::shared_ptr<LoxInstance>>(object)->Set(val.name, std::move(value));
	return value;
}

std::any Interpreter::Visit(const ast::expr::Super& val)
{
	const auto it = m_locals.find(val);
	if (it == std::end(m_locals))
		return {};
	auto sup = m_environment->GetAt(it->second, "super");
	if (CheckAnyType<std::shared_ptr<LoxClass>>(sup))
	{
		auto obj = m_environment->GetAt(it->second - 1, "this");
		if (CheckAnyType<std::shared_ptr<LoxInstance>>(obj))
		{
			auto method = std::any_cast<std::shared_ptr<LoxClass>>(sup)
				->FindMethod(val.method.m_lexeme);

			if (!method)
			{
				throw RuntimeError(val.method,
					"Undefined property' " + val.method.m_lexeme + "'.");
			}

			method->Bind(std::any_cast<std::shared_ptr<LoxInstance>>(obj));
			return static_pointer_cast<LoxCallable>(method);
		}
	}
	return {};
}

std::any Interpreter::Visit(const ast::expr::This& val)
{
	return LookUpVariable(val.keyword, val);
}

std::any Interpreter::Visit(const ast::expr::Unary& val)
{
	auto right = Evaluate(*val.right);
	switch (val.op.m_type)
	{
	case TokenType::BANG:
		return IsTruthy(right);
	case TokenType::MINUS:
		CheckNumberOperand(val.op, right);
		return -std::any_cast<double>(right);
	}
	return {};
}

void Interpreter::Execute(const ast::stmt::Stmt& stmt)
{
	stmt.Accept(*this);
}

void Interpreter::Resolve(const ast::expr::Expr& expr, int depth)
{
	m_locals.emplace(expr, depth);
}

void Interpreter::ExecuteBlock(const std::vector<ast::stmt::StmtPtr>& statements, std::shared_ptr<Environment> environment)
{
	auto prev = m_environment;
	m_environment = std::move(environment);
	SCOPE_EXIT{ m_environment = std::move(prev); };
	for (const auto& statement : statements)
		Execute(*statement);
}

std::any Interpreter::Evaluate(const ast::expr::Expr& expr)
{
	return expr.Accept(*this);
}

bool Interpreter::IsTruthy(const std::any& val)
{
	if (!val.has_value())
		return false;
	if (CheckAnyType<bool>(val))
		return std::any_cast<bool>(val);
	return true;
}

bool Interpreter::IsEqual(const std::any& left, const std::any& right)
{
	if (!left.has_value() && !right.has_value())
		return true;
	if (!left.has_value())
		return false;
	if (left.type() == right.type())
	{
		if (CheckAnyType<bool>(left))
			return std::any_cast<bool>(left) == std::any_cast<bool>(right);
		if (CheckAnyType<double>(left))
			return std::any_cast<double>(left) == std::any_cast<double>(right);
		if (CheckAnyType<std::string>(left))
			return GetStringRef(left) == GetStringRef(right);
		return false;
	}
	return false;

}

void Interpreter::CheckNumberOperand(const Token& op, const std::any& operand)
{
	if (CheckAnyType<double>(operand))
		return;
	throw RuntimeError(op, "Operand must be a number.");
}

void Interpreter::CheckNumberOperands(const Token& op, const std::any& left, const std::any& right)
{
	if (CheckAnyType<double>(left) && CheckAnyType<double>(right))
		return;
	throw RuntimeError(op, "Operands must be numbers.");
}

std::string Interpreter::Stringify(const std::any& val)
{
	if (!val.has_value())
		return "nil";
	if (CheckAnyType<double>(val))
		return std::to_string(std::any_cast<double>(val));
	if (CheckAnyType<std::string>(val))
		return GetStringRef(val);
	if (CheckAnyType<bool>(val))
	{
		std::stringstream ss;
		ss << std::boolalpha << std::any_cast<bool>(val);
		return ss.str();
	}
	if (CheckAnyType<std::shared_ptr<LoxCallable>>(val))
		return std::any_cast<std::shared_ptr<LoxCallable>>(val)->ToString();
	if (CheckAnyType<std::shared_ptr<LoxClass>>(val))
		return std::any_cast<std::shared_ptr<LoxClass>>(val)->ToString();
	if (CheckAnyType<std::shared_ptr<LoxInstance>>(val))
		return std::any_cast<std::shared_ptr<LoxInstance>>(val)->ToString();

	//should be unreacable
	throw std::runtime_error("Can`t stringify type.");
}

Interpreter::Interpreter()
	: m_globals(std::make_shared<Environment>())
	, m_environment(m_globals)
{
	m_globals->Define("clock", static_pointer_cast<LoxCallable>(std::make_shared<Clock>()));
}

void Interpreter::Interpret(const std::vector<ast::stmt::StmtPtr>& statements) try
{
	for (const auto& stmt : statements)
		Execute(*stmt);
}
catch (const RuntimeError& err)
{
	HandleRuntimeError(err);
}