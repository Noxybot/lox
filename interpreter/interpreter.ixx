export module interpreter;

import ast;
import core;
import log;

import <stdexcept>;
import <string>;
import <sstream>;
import <typeinfo>;
import <variant>;

export class Interpreter : public ast::Visitor
{
	std::any Visit(const ast::Binary& val) override;
	std::any Visit(const ast::Grouping& val) override;
	std::any Visit(const ast::Literal& val) override;
	std::any Visit(const ast::Unary& val) override;

	std::any Evaluate(const ast::Expr& expr);
	static bool IsTruthy(const std::any& val);
	static bool IsEqual(const std::any& left, const std::any& right);
	static void CheckNumberOperand(const Token& op, const std::any& operand);
	static void CheckNumberOperands(const Token& op, const std::any& left, const std::any& right);
	static std::string Stringify(const std::any& val);
public:
	void Interpret(const ast::Expr& expr);
};

module :private;

template<class T>
bool CheckAnyType(const std::any& val) try
{
	(void)std::any_cast<T>(val);
	return true;
}
catch (...)
{
	return false;
}


std::any Interpreter::Visit(const ast::Binary& val)
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
			return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
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
}


std::any Interpreter::Visit(const ast::Grouping& val)
{
	return Evaluate(*val.expression);
}

std::any Interpreter::Visit(const ast::Literal& val)
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

std::any Interpreter::Visit(const ast::Unary& val)
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

std::any Interpreter::Evaluate(const ast::Expr& expr)
{
	return expr.Accept(*this);
}

bool Interpreter::IsTruthy(const std::any& val)
{
	if (!val.has_value())
		return false;
	if (CheckAnyType<bool>(val))
		return std::any_cast<bool>(val);
	return false;
}

bool Interpreter::IsEqual(const std::any& left, const std::any& right)
{
	if (!left.has_value() && !right.has_value())
		return true;
	if (!left.has_value())
		return false;
	if (left.type().hash_code() == right.type().hash_code())
	{
		if (CheckAnyType<bool>(left))
			return std::any_cast<bool>(left) == std::any_cast<bool>(right);
		if (CheckAnyType<double>(left))
			return std::any_cast<double>(left) == std::any_cast<double>(right);
		if (CheckAnyType<std::string>(left))
			return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
		if (CheckAnyType<double>(left))
			return std::any_cast<double>(left) == std::any_cast<double>(right);
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
		return std::any_cast<std::string>(val);
	if (CheckAnyType<bool>(val))
	{
		std::stringstream ss;
		ss << std::boolalpha << std::any_cast<bool>(val);
		return ss.str();
	}
	//should be unreacable
	throw std::runtime_error("Can`t stringify type.");
}

void Interpreter::Interpret(const ast::Expr& expr) try
{
	const auto val = Evaluate(expr);
	std::cout << Stringify(val) << std::endl;
}
catch (const RuntimeError& err)
{
	HandleRuntimeError(err);
}