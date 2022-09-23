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
	Environment& Ancestor(int distance)
	{
		auto* res = this;
		for (int i = 0; i < distance; ++i)
			res = res->m_enclosing.get();
		return *res;
	}
	std::any GetAt(int distance, std::string_view name)
	{
		const auto& ancestor = Ancestor(distance);
		const auto it = ancestor.m_values.find(std::string(name));
		if (it != std::end(ancestor.m_values))
			return it->second;
		return {};
	}
	void AssignAt(int distance, const Token& name, std::any val)
	{
		Ancestor(distance).m_values[name.m_lexeme] = std::move(val);
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
	void Assign(const Token& name, std::any val)
	{
		auto it = m_values.find(name.m_lexeme);
		if (it != std::end(m_values))
		{
			it->second = std::move(val);
			return;
		}
		if (m_enclosing)
		{
			m_enclosing->Assign(name, std::move(val));
			return;
		}
		throw RuntimeError(name, "Undefined variable'" + name.m_lexeme + "'.");
	}
};



