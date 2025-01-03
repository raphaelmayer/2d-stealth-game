#pragma once

#include "../../components/Positionable.hpp"
#include "../../components/Rotatable.hpp"
#include "../../components/Vision.hpp"
#include "../../constants.hpp"
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
			BT::InputPort<Entity>("entity"),
			BT::InputPort<Rotation>("direction")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Entity> entity = getInput<Entity>("entity");
		BT::Expected<Rotation> direction = getInput<Rotation>("direction");

		if (!entity)
			return BT::NodeStatus::FAILURE;

		const auto &position = ecs.getComponent<Positionable>(entity.value()).position;

		// TODO: Move code below with calculateDirection function to to IsEnemyVisible component. TurnTo should only get
		// direction and turn accordingly.
		// const Entity &otherEntity = ecs.getComponent<Vision>(entity.value()).visibleEntities[0];
		// const Vec2d &otherPos = ecs.getComponent<Positionable>(otherEntity).position;

		auto &rotation = ecs.getComponent<Rotatable>(entity.value()).rotation;
		// rotation = calculateDirection(position, otherPos, rotation);

		rotation = direction.value();

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