#pragma once

#include <memory>
#include "TreeNode.hpp"
#include "Blackboard.hpp"

class DecoratorNode : public TreeNode {
  public:
	std::unique_ptr<TreeNode> child;

	void setChild(std::unique_ptr<TreeNode> child) { this->child = std::move(child); }
};

class InverterNode : public DecoratorNode {
	BehaviorStatus tick(Blackboard &blackboard) override { 
		const auto &status = this->child->tick(blackboard);

		switch (status) {
		case BehaviorStatus::FAILURE:
			return BehaviorStatus::SUCCESS;
		break;
		case BehaviorStatus::SUCCESS:
			return BehaviorStatus::FAILURE;
			break;
		default:
			return status;
			break;
		}
	}
};