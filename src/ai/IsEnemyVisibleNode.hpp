#pragma once

#include "../behaviortree/BehaviorStatus.hpp"
#include "../behaviortree/TreeNode.hpp"
#include "../behaviortree/LeafNode.hpp"
#include "../components/AI.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Vec2d.hpp"

class IsEnemyVisibleNode : public ConditionNode {
  protected:
	BehaviorStatus tick(Blackboard &blackboard) override
	{
		ECSManager &ecs = blackboard.ecs;
		Entity entity = blackboard.entity;

		if (!ecs.hasComponent<Vision>(entity)) {
			return BehaviorStatus::FAILURE;
		}

		const Vision &vision = ecs.getComponent<Vision>(entity);

		if (vision.visibleEntities.size() > 0) {
			Entity other = vision.visibleEntities[0];
			auto otherPos = ecs.getComponent<Positionable>(other).position;
			
			// TODO: pass this entity or position to next node
			ecs.getComponent<AI>(entity).targetPosition = otherPos;

			return BehaviorStatus::SUCCESS;
		}

		return BehaviorStatus::FAILURE;
	}

	void halt() {
		// TODO
	}
};