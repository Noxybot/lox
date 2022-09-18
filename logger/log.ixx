module;
#include <iostream>

export module log;

import <string_view>;
import <string>;

export bool has_error = false;

export void Report(int line, std::string_view where, std::string_view message)
{
	std::cerr << "[line " << std::to_string(line) <<
		" ] Error" << where << ": " << message << '\n';
	has_error = true;
}

export void Error(int line, std::string_view message)
{
	Report(line, "", message);
}
