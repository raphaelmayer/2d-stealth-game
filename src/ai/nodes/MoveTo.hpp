#pragma once

#include "../../components/AI.hpp"
#include "../../components/Positionable.hpp"
#include "../../components/RigidBody.hpp"
#include "../../ecs/ECSManager.hpp"
#include "../../engine/Vec2d.hpp"
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
			BT::InputPort<Vec2d>("position")
		};
		// clang-format on
	}

	BT::NodeStatus onStart() override
	{
		// TODO
		return BT::NodeStatus::RUNNING;
	}

	BT::NodeStatus onRunning() override
	{
		BT::Expected<Entity> entity = getInput<Entity>("entity");
		BT::Expected<Vec2d> pos = getInput<Vec2d>("position");

		if (!entity)
			return BT::NodeStatus::FAILURE;

		auto &ai = ecs.getComponent<AI>(entity.value());
		auto &rigidBody = ecs.getComponent<RigidBody>(entity.value());
		const Vec2d &position = ecs.getComponent<Positionable>(entity.value()).position;

		ai.targetPosition = pos.value();

		if (position == ai.targetPosition)
			return BT::NodeStatus::SUCCESS;

		return BT::NodeStatus::RUNNING;
	}

	void onHalted() override
	{
		// nothing to do here...
		std::cout << "MoveTo node interrupted" << std::endl;
	}

  private:
	ECSManager &ecs;
};