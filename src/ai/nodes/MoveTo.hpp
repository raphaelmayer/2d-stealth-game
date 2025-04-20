#pragma once

#include "../../components/Pathfinding.hpp"
#include "../../components/Positionable.hpp"
#include "../../components/RigidBody.hpp"
#include "../../ecs/ECSManager.hpp"
#include "../../engine/types/Vec2f.hpp"
#include "../../modules/AStar.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <iostream>
#include <string>

class MoveTo : public BT::StatefulActionNode {
  public:
	MoveTo(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::StatefulActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity"),
			BT::InputPort<Vec2f>("position")
		};
		// clang-format on
	}

	BT::NodeStatus onStart() override
	{
		BT::Expected<Entity> expEntity = getInput<Entity>("entity");
		BT::Expected<Vec2f> expPosition = getInput<Vec2f>("position");

		if (!expEntity || !expPosition)
			return BT::NodeStatus::FAILURE;

		entity = expEntity.value();
		targetPosition = expPosition.value();

		Vec2i &position = ecs.getComponent<Pathfinding>(entity).targetPosition;
		position = Utils::toInt(targetPosition);

		return BT::NodeStatus::RUNNING;
	}

	BT::NodeStatus onRunning() override
	{
		const Vec2f &currentPosition = ecs.getComponent<Positionable>(entity).position;
		if (currentPosition == targetPosition) {
			return BT::NodeStatus::SUCCESS;
		}

		return BT::NodeStatus::RUNNING;
	}

	void onHalted() override
	{
		// nothing to do here...
		std::cout << "MoveTo node interrupted" << std::endl;
	}

  private:
	ECSManager &ecs;

	Entity entity;
	Vec2f targetPosition;
};