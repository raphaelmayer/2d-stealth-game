#pragma once

#include "../behaviortree/BehaviorStatus.hpp"
#include "../behaviortree/TreeNode.hpp"
#include "../behaviortree/LeafNode.hpp"
#include "../components/AI.hpp"
#include "../components/Patrol.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Vec2d.hpp"
#include "../constants.hpp"

class TurnToNode : public ActionNode {
  protected:
	BehaviorStatus tick(Blackboard &blackboard) override
	{
		ECSManager &ecs = blackboard.ecs;
		Entity entity = blackboard.entity;

		if (blackboard.accumulator != 0)
			return BehaviorStatus::SUCCESS;

		if (!ecs.hasComponent<Positionable>(entity) || !ecs.hasComponent<RigidBody>(entity)
		    || !ecs.hasComponent<Rotatable>(entity)) {
			return BehaviorStatus::FAILURE;
		}

		auto &position = ecs.getComponent<Positionable>(entity).position;
		auto &rigidBody = ecs.getComponent<RigidBody>(entity);
		auto &rotation = ecs.getComponent<Rotatable>(entity).rotation;
		auto &ai = ecs.getComponent<AI>(entity);
		
		if (rigidBody.isMoving) {
			return BehaviorStatus::FAILURE;
		}

		Vec2d &otherPos = ai.targetPosition;
		
		if (otherPos == Vec2d{-1, -1}) {
			rotation = static_cast<Rotation>((rotation + 1) % 4);
			return BehaviorStatus::SUCCESS;
		}
		
		rotation = calculateDirection(position, otherPos, rotation); 
		otherPos = Vec2d{-1, -1};

		return BehaviorStatus::SUCCESS;
	}

	void halt()
	{
		// TODO
	}

  private:
	Rotation calculateDirection(Vec2d start, Vec2d end, Rotation currentRotation)
	{
		// Calculate the difference between the points
		double dx = end.x - start.x;
		double dy = end.y - start.y;

		// Normalize the direction vector
		double magnitude = std::sqrt(dx * dx + dy * dy);
		if (magnitude == 0) {
			return currentRotation; // Default direction for no movement
		}
		dx /= magnitude;
		dy /= magnitude;

		// Determine direction based on dominant axis
		if (std::abs(dx) > std::abs(dy)) {
			return dx > 0 ? Rotation::EAST : Rotation::WEST;
		} else {
			return dy > 0 ? Rotation::SOUTH : Rotation::NORTH;
		}
	}
};