export module interpreter:environment;

import core;
import log;

import <any>;
import <string>;
import <memory>;
import <unordered_map>;

export class Environment
{
	std::unordered_map<std::string, std::any> m_values;
	std::shared_ptr<Environment> m_enclosing;
public:
	Environment() = default;
	explicit Environment(std::shared_ptr<Environment> enclosing)
		: m_enclosing(std::move(enclosing))
	{}
	void Define(std::string_view name, const std::any& value)
	{
		auto res = m_values.emplace(name, value);
		if (!res.second)
			res.first->second = value;
	}
	const std::any& Get(const Token& name)
	{
		const auto it = m_values.find(name.m_lexeme);
		if (it != std::end(m_values))
			return it->second;
		if (m_enclosing)
			return m_enclosing->Get(name);
		throw RuntimeError(name, "Undefined variable '" + name.m_lexeme + "'.");
	}
	void Assign(const Token& name, const std::any& val)
	{
		auto it = m_values.find(name.m_lexeme);
		if (it != std::end(m_values))
		{
			it->second = val;
			return;
		}
		if (m_enclosing)
		{
			m_enclosing->Assign(name, val);
			return;
		}
		throw RuntimeError(name, "Undefined variable'" + name.m_lexeme + "'.");
	}
};



