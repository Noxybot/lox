#pragma once

#include "Visitor.h"

#include <memory>

namespace ast
{
class Node
{
public:
	virtual ~Node() = default;
	virtual int Accept(Visitor& visitor) const = 0;
protected:
	static void Error(const char* error)
	{
		throw std::exception(error);
	}
};

using NodePtr = std::shared_ptr<Node>;
}
