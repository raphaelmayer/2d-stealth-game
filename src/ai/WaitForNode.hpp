#pragma once

#include "../behaviortree/BehaviorStatus.hpp"
#include "../behaviortree/TreeNode.hpp"
#include "../behaviortree/LeafNode.hpp"
#include "../components/AI.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Vec2d.hpp"

class WaitForNode : public ActionNode {
  protected:
	BehaviorStatus tick(Blackboard &blackboard) override
	{
		ECSManager &ecs = blackboard.ecs;
		Entity entity = blackboard.entity;

		if (blackboard.accumulator >= blackboard.timeout) {
			blackboard.accumulator = 0;
			return BehaviorStatus::SUCCESS;
		}
		
		blackboard.accumulator += blackboard.deltaTime;

		return BehaviorStatus::RUNNING;
	}

	void halt()
	{
		// TODO
	}
};