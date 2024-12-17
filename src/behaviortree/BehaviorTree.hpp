#pragma once

#include <memory>
#include "BehaviorStatus.hpp"
#include "Blackboard.hpp"
#include "TreeNode.hpp"

/*
	This custom BT implementation is unfinished and not currently in use. We use BehaviortreeCPP for now.
*/
class BehaviorTree {
  public:
	explicit BehaviorTree(std::unique_ptr<TreeNode> node) : root(std::move(node)) {}

	BehaviorStatus tick(Blackboard &blackboard) { return root->internalTick(blackboard); }
  
	void halt() {
		// TODO: halt children
	}


  private:
	std::unique_ptr<TreeNode> root;
};
