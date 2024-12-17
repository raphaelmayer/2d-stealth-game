#pragma once

#include <memory>
#include <vector>
#include "BehaviorStatus.hpp"
#include "Blackboard.hpp"
#include "TreeNode.hpp"

class ControlNode : public TreeNode {
  protected:
	std::vector<std::unique_ptr<TreeNode>> children;

  public:
	virtual TreeNodeType type() const override final { return TreeNodeType::CONTROL; }

	void addChild(std::unique_ptr<TreeNode> child) { children.push_back(std::move(child)); }

	size_t childCount() const { return children.size(); }

	void halt() override { 
		resetChildren();
		resetStatus();
	}

	void haltChild(size_t i) { 
		auto status = children[i]->status();

		if (status == BehaviorStatus::RUNNING) {
			children[i]->haltNode();
		}
	}

	void haltChildren() { 
		for (size_t i = 0; i < childCount(); i++) {
			haltChild(i);
		}
	}

	void resetChildren()
	{
		for (auto &child : children) {
			if (child->status() == BehaviorStatus::RUNNING)
				child->haltNode();
			child->resetStatus();
		}
	}
};

class SequenceNode : public ControlNode {
  protected:
	BehaviorStatus tick(Blackboard &blackboard) override
	{
		for (auto &child : this->children) {
			BehaviorStatus state = child->internalTick(blackboard);

			if (state == BehaviorStatus::FAILURE)
				return BehaviorStatus::FAILURE;
			if (state == BehaviorStatus::RUNNING)
				return BehaviorStatus::RUNNING;
		}

		return BehaviorStatus::SUCCESS;
	}

	void halt() { ControlNode::halt(); }
};

class SelectorNode : public ControlNode {
  protected:
	BehaviorStatus tick(Blackboard &blackboard) override
	{
		for (auto &child : this->children) {
			BehaviorStatus state = child->internalTick(blackboard);

			if (state == BehaviorStatus::SUCCESS)
				return BehaviorStatus::SUCCESS;
			if (state == BehaviorStatus::RUNNING)
				return BehaviorStatus::RUNNING;
		}

		return BehaviorStatus::FAILURE;
	}

	void halt() { ControlNode::halt(); }
};