#pragma once

#include "TreeNode.hpp"

class LeafNode : public TreeNode {
	
};

class ActionNode : public LeafNode {
  public:
	TreeNodeType type() const override final { return TreeNodeType::ACTION; }
};

class ConditionNode : public LeafNode {
  public:
	TreeNodeType type() const override final { return TreeNodeType::CONDITION; }
};