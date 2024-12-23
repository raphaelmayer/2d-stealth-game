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

class WaitFor : public BT::SyncActionNode {
  public:
	WaitFor(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity"),
			BT::InputPort<double>("deltaTime")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Entity> entity = getInput<Entity>("entity");
		BT::Expected<double> deltaTime = getInput<double>("deltaTime");

		if (!entity || !deltaTime)
			return BT::NodeStatus::FAILURE;

		if (acc >= maxTime) {
			acc = 0;
			return BT::NodeStatus::SUCCESS;
		} else {
			acc += deltaTime.value();
			return BT::NodeStatus::FAILURE;
		}
	}

  private:
	ECSManager &ecs;
	// TODO: acc does not really work here, since it persists and only resets when acc >= maxTime.
	// We'll probably resort to saving it in some ecs component.
	double acc = 0;
	const double maxTime = 1;
};