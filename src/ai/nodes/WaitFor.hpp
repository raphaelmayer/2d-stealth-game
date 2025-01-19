#pragma once

#include "../../components/Positionable.hpp"
#include "../../components/Rotatable.hpp"
#include "../../components/Vision.hpp"
#include "../../ecs/ECSManager.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <string>

class WaitFor : public BT::StatefulActionNode {
  public:
	WaitFor(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::StatefulActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity"),
			BT::InputPort<double>("deltaTime"),
			BT::InputPort<double>("duration")
		};
		// clang-format on
	}

	BT::NodeStatus onStart() override
	{
		acc = 0;
		return BT::NodeStatus::RUNNING;
	}

	BT::NodeStatus onRunning() override
	{
		BT::Expected<Entity> entity = getInput<Entity>("entity");
		BT::Expected<double> deltaTime = getInput<double>("deltaTime");
		BT::Expected<double> duration = getInput<double>("duration");

		if (!entity || !deltaTime)
			return BT::NodeStatus::FAILURE;

		if (acc <= duration.value()) {
			acc += deltaTime.value();
			return BT::NodeStatus::RUNNING;
		}

		return BT::NodeStatus::SUCCESS;
	}

	void onHalted() override
	{
		// nothing to do here...
		std::cout << "WaitFor node interrupted" << std::endl;
	}

  private:
	ECSManager &ecs;
	// TODO: acc does not really work here, since it persists and only resets when acc >= maxTime.
	// We'll probably resort to saving it in some ecs component.
	double acc = 0;
};