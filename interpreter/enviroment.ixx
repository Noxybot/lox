export module interpreter:environment;

import core;
import log;

import <any>;
import <string>;
import <memory>;
import <unordered_map>;

template<class T>
bool CheckAnyType(const std::any& val);

export class Environment
{
	std::unordered_map<std::string, std::any> m_values;
	std::shared_ptr<Environment> m_enclosing;
	std::unordered_map<std::string, std::string> m_strings_cache;
public:
	Environment() = default;
	explicit Environment(std::shared_ptr<Environment> enclosing)
		: m_enclosing(std::move(enclosing))
	{}
	void Define(std::string_view name, std::any value)
	{
		if (CheckAnyType<std::string>(value))
			Define(name, std::move(std::any_cast<std::string&>(value)));
		else
			m_values.insert(std::make_pair(std::string(name), std::move(value)));
	}
	void Define(std::string_view name, std::string str)
	{
		m_strings_cache.insert(std::make_pair(std::string(name), std::move(str)));
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
	std::any Get(const Token& name)
	{
		const auto str_it = m_strings_cache.find(name.m_lexeme);
		if (str_it != std::end(m_strings_cache))
		{
			return std::reference_wrapper<const std::string>(str_it->second);
		}

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
	const std::shared_ptr<Environment>& GetEnclosing() const
	{
		return m_enclosing;
	}
};



