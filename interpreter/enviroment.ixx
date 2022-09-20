export module interpreter:environment;

import core;
import log;

import <any>;
import <string>;
import <unordered_map>;

export class Environment
{
	std::unordered_map<std::string, std::any> m_values;
public:
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
		throw RuntimeError(name, ("Undefined variable '" + name.m_lexeme + "'.").c_str());
	}
};



