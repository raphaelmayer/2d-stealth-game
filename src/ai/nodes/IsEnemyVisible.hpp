#pragma once

#include "../../components/Vision.hpp"
#include "../../ecs/ECSManager.hpp"
#include "../../engine/types/Vec2f.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig

#include <string>

// I guess we could set a vector of all visible enemies (or just a check, since we could reuse
// visibleEnemies in later nodes) and let succeeding nodes handle their specifics like who to target etc.
//
// The fundamental question is, if we want to check our high-level state or if an enemy is visible.

class IsEnemyVisible : public BT::SyncActionNode {
  public:
	IsEnemyVisible(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity"),
			BT::OutputPort<Entity>("otherEntity"),
			BT::OutputPort<Vec2f>("otherPosition"),
			BT::OutputPort<Rotation>("direction")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		Entity entity = getInputOrThrow<Entity>("entity");
		const Vision &vision = ecs.getComponent<Vision>(entity);

		if (vision.visibleEnemies.empty()) {
			return BT::NodeStatus::FAILURE;
		}

		const Vec2f &position = ecs.getComponent<Positionable>(entity).position;
		const Rotation &rotation = ecs.getComponent<Rotatable>(entity).rotation;
		const Entity &otherEntity = vision.visibleEnemies[0];
		const Vec2f &otherPosition = ecs.getComponent<Positionable>(otherEntity).position;

		// TODO: Is this actually the best place to do this?
		Rotation direction = calculateDirection(position, otherPosition, rotation);

		setOutput<Entity>("otherEntity", otherEntity);
		setOutput<Vec2f>("otherPosition", otherPosition);
		setOutput<Rotation>("direction", direction);

		return BT::NodeStatus::SUCCESS;
	}

  private:
	// TODO: Implement a base class, which implements this function? Syntax is kind of neat.
	template <typename T>
	T getInputOrThrow(const std::string &key)
	{
		BT::Expected<T> exp = getInput<T>(key);

		if (!exp)
			throw std::runtime_error("Node input \"" + key + "\" could not be found.");

		return exp.value();
	}

	Rotation calculateDirection(const Vec2f &start, const Vec2f &end, const Rotation &currentRotation)
	{
		// Calculate the difference between the points
		float dx = end.x - start.x;
		float dy = end.y - start.y;

		// Normalize the direction vector
		float magnitude = std::sqrt(dx * dx + dy * dy);
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