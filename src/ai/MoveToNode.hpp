#pragma once

#include "../behaviortree/BehaviorStatus.hpp"
#include "../behaviortree/TreeNode.hpp"
#include "../behaviortree/LeafNode.hpp"
#include "../components/AI.hpp"
#include "../components/Patrol.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Vec2d.hpp"

class MoveToNode : public ActionNode {
  protected:
	BehaviorStatus tick(Blackboard &blackboard) override { 
		ECSManager &ecs = blackboard.ecs;
		Entity entity = blackboard.entity;

		if (!ecs.hasComponent<Positionable>(entity) || !ecs.hasComponent<RigidBody>(entity)) {
			return BehaviorStatus::FAILURE;
		}

		auto &positionable = ecs.getComponent<Positionable>(entity);
		auto &rigidBody = ecs.getComponent<RigidBody>(entity);
		auto &ai = ecs.getComponent<AI>(entity);

		// Check if the entity has reached the target position
		if (positionable.position == rigidBody.endPosition) {
			return BehaviorStatus::SUCCESS;
		}

		// If the entity is currently moving, return Running
		if (rigidBody.isMoving) {
			return BehaviorStatus::RUNNING;
		}

		// Initiate movement
		rigidBody.startPosition = positionable.position;
		rigidBody.endPosition = ai.targetPosition;
		rigidBody.isMoving = true;

		// Optionally reset movement parameters
		rigidBody.progress = 0;
		rigidBody.accumulator = 0;

		return BehaviorStatus::RUNNING;
	}

	void halt()
	{
		// TODO
	}
};