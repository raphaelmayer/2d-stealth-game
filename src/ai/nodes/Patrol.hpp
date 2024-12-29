#pragma once

#include "../../components/AI.hpp"
#include "../../components/Patrol.hpp"
#include "../../components/Positionable.hpp"
#include "../../ecs/ECSManager.hpp"
#include "../../engine/Vec2d.hpp"
#include "../../modules/AStar.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <string>

class Patrol : public BT::SyncActionNode {
  public:
	Patrol(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Entity> entity = getInput<Entity>("entity");

		if (!entity)
			return BT::NodeStatus::FAILURE;

		if (!ecs.hasComponent<Patrol>(entity.value()))
			return BT::NodeStatus::FAILURE;

		const auto patrol = ecs.getComponent<Patrol>(entity.value());
		const auto ai = ecs.getComponent<AI>(entity.value());
		const auto &position = ecs.getComponent<Positionable>(entity.value()).position;

		// Reached patrol point, set next one.
		if (position.toTileSize() == patrol.waypoints[patrol.patrolIndex]) {
			bool someChance = false;
			if (someChance) {
				// Make AI less predictable. Skip patrolling for the time being and do something else.
				// I guess this Chance thing could also be a separate (reusable) leaf or control node.
				return BT::NodeStatus::FAILURE;
			}

			if ("acc" < patrol.waypoints[patrol.patrolIndex].duration) {
				// TODO: Wait here or leverage WaitFor node?
				// acc += deltaTime;
				// return BT::NodeStatus::SUCCESS;
			}

			patrol.patrolIndex += 1;
			ai.targetPosition = patrol.waypoints[patrol.patrolIndex];
			// Does this even make sense? This could also be in the PhysicsSystem.
			ai.path = AStar::findPath(map, start, target);
			rigidBody.endPosition = ai.path[1];
			rigidBody.isMoving = true;
		}

		// Does this even make sense? This could also be in the PhysicsSystem.
		if (ai.path.empty() && ai.targetPosition != Vec2d{-1, -1}) {
			ai.path = AStar::findPath(map, start, target);
		}

		return BT::NodeStatus::SUCCESS;
	}

  private:
	AStar astar;
	ECSManager &ecs;
};