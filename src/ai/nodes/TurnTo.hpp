#pragma once

#include "../../components/Positionable.hpp"
#include "../../components/Rotatable.hpp"
#include "../../components/Vision.hpp"
#include "../../ecs/ECSManager.hpp"
#include "../../engine/Vec2d.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <string>

class TurnTo : public BT::SyncActionNode {
  public:
	TurnTo(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity")
			//BT::InputPort<Vec2d>("position")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Entity> entity = getInput<Entity>("entity");
		// BT::Expected<Vec2d> position = getInput<Vec2d>("position");

		if (!entity)
			return BT::NodeStatus::FAILURE;

		// We might need a check like this, if we change our approach (i.e. no condition node)
		/*	if (ecs.hasComponent<AI>(entity) && ecs.hasComponent<Vision>(entity)) {
		    return BT::NodeStatus::FAILURE;
		}*/

		// Vec2d &otherPos = getInput<Vec2d>("position"); // TODO: read from blackboard or maybe decide here?
		const auto &position = ecs.getComponent<Positionable>(entity.value()).position;
		const Entity &otherEntity = ecs.getComponent<Vision>(entity.value()).visibleEntities[0];
		const Vec2d &otherPos = ecs.getComponent<Positionable>(otherEntity).position;
		auto &rotation = ecs.getComponent<Rotatable>(entity.value()).rotation;

		rotation = calculateDirection(position, otherPos, rotation);

		return BT::NodeStatus::SUCCESS;
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

	ECSManager &ecs;
};