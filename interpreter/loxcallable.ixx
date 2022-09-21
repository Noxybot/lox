export module interpreter:loxcallable;

import ast;
import interpreter;
import :environment;

import <any>;
import <chrono>;
import <memory>;
import <vector>;

export struct Return
{
public:
	std::any m_val;
	explicit Return(std::any val)
		: m_val(std::move(val))
	{}
};

export class LoxCallable
{
public:
	virtual ~LoxCallable() = default;
	virtual int Arity() const = 0;
	virtual std::any Call(Interpreter& interpreter, const std::vector<std::any>& arguments) = 0;
	virtual std::string ToString() const = 0;
};

export class Clock : public LoxCallable
{
	int Arity() const override { return 0; }
	std::any Call(Interpreter& interpreter, const std::vector<std::any>& arguments) override
	{
		return static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>
			(std::chrono::steady_clock::now().time_since_epoch()).count());
	}
	std::string ToString() const override { return "<native fn>"; }

};

export class LoxFunction : public LoxCallable
{
	//Should be safe, because AST lifetime same as program`s
	const ast::stmt::Function& m_declaration;
	std::shared_ptr<Environment> m_closure;
public:
	explicit LoxFunction(const ast::stmt::Function& function, std::shared_ptr<Environment> closure)
		: m_declaration(function)
		, m_closure(std::move(closure))
	{}
	int Arity() const override
	{
		return m_declaration.params.size();
	}
	std::any Call(Interpreter& interpreter, const std::vector<std::any>& arguments) override
	{
		auto environment = std::make_shared<Environment>(m_closure);
		
		for (int i = 0; i < m_declaration.params.size(); ++i)
		{
			environment->Define(m_declaration.params[i].m_lexeme,
				arguments[i]);
		}
		try
		{
			interpreter.ExecuteBlock(m_declaration.body, std::move(environment));
		}
		catch (Return& return_value)
		{
			return std::move(return_value.m_val);
		}
		return {};
	}
	std::string ToString() const override
	{
		return "<fn " + m_declaration.name.m_lexeme + ">";
	}
};
