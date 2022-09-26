export module interpreter:loxclass;

import core;
import :loxcallable;

import <any>;
import <string>;
import <memory>;
import <unordered_map>;

class LoxInstance;

export class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass>
{
	friend class LoxInstance;
	const std::string m_name;
	const std::shared_ptr<LoxClass> m_superclass;
	std::unordered_map<std::string, std::shared_ptr<LoxFunction>> m_methods;
public:
	explicit LoxClass(std::string name,
		std::shared_ptr<LoxClass> superclass,
		std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods)
		: m_name(std::move(name))
		, m_superclass(std::move(superclass))
		, m_methods(std::move(methods))
	{}
	int Arity() const
	{
		auto initializer = FindMethod("init");
		if (!initializer)
			return 0;
		return initializer->Arity();
	}

	std::any Call(Interpreter& interpreter, const std::vector<std::any>& arguments) override
	{
		const auto instance =
			std::make_shared<LoxInstance>(shared_from_this());
		const auto initializer = FindMethod("init");
		if (initializer)
			initializer->Bind(instance)->Call(interpreter, arguments);
		return instance;
	}

	std::string ToString() const
	{
		return m_name;
	}

	std::shared_ptr<LoxFunction> FindMethod(const std::string& name) const
	{
		const auto it = m_methods.find(name);
		if (it != std::end(m_methods))
			return it->second;
		if (m_superclass)
			return m_superclass->FindMethod(name);
		return nullptr;
	}
};

export class LoxInstance : public std::enable_shared_from_this<LoxInstance>
{
	const std::shared_ptr<LoxClass> m_class;
	std::unordered_map<std::string, std::any> m_fields;
public:
	explicit LoxInstance(std::shared_ptr<LoxClass> klass)
		: m_class(std::move(klass))
	{}
	std::string ToString() const
	{
		return m_class->m_name + " instance";
	}
	std::any Get(const Token& name)
	{
		const auto it = m_fields.find(name.m_lexeme);
		if (it != std::end(m_fields))
			return it->second;
		auto method = m_class->FindMethod(name.m_lexeme);
		if (method)
			return std::static_pointer_cast<LoxCallable>(method->Bind(shared_from_this()));
		throw RuntimeError(name, "Undefined property'" + name.m_lexeme + "'.");
	}
	void Set(const Token& name, std::any value)
	{
		m_fields[name.m_lexeme] = std::move(value);
	}
};