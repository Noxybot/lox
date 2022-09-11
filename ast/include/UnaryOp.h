#pragma once

#include "Node.h"
#include "TokenTypes.h"

namespace ast
{
class UnaryOp : public Node
{
	TokenType m_op_type;
	NodePtr m_child;
public:
	UnaryOp(TokenType op_type, NodePtr node);
	int Accept(Visitor& visitor) const override;
	TokenType GetOpType() const;
	const Node& GetChild() const;
};
}