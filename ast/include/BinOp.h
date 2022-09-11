#pragma once

#include "Node.h"
#include "TokenTypes.h"

#include <memory>

namespace ast
{

class BinOp : public Node
{
	NodePtr m_left, m_right;
	TokenType m_op_type;
public:
	BinOp(NodePtr left, TokenType op_type, NodePtr right);
	int Accept(Visitor& visitor) const override;
	TokenType GetOpType() const;
	const Node& GetLeftNode() const;
	const Node& GetRightNode() const;
};

}