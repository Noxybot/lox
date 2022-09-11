#pragma once

#include "Node.h"
#include "Token.h"

namespace ast
{

class Num : public Node
{
	int m_val = 0;
public:
	explicit Num(const Token& token);
	int Accept(Visitor& visitor) const override;
	int GetNum() const;
};

}