#pragma once

#include "../../ai/AIState.hpp"
#include "../../components/AI.hpp"
#include "../../components/Positionable.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <easys/easys.hpp>
#include <iostream>
#include <string>

// I guess we could set a vector of all visible enemies (or just a check, since we could reuse
// visibleEntities in later nodes) and let succeeding nodes handle their specifics like who to target etc.
//
// The fundamental question is, if we want to check our high-level state or if an enemy is visible.

class IsInState : public BT::SyncActionNode {
  public:
	IsInState(const std::string &name, const BT::NodeConfig &config, Easys::ECS &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Easys::Entity>("entity"),
			BT::InputPort<AIState>("state")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Easys::Entity> entity = getInput<Easys::Entity>("entity");
		BT::Expected<AIState> requiredState = getInput<AIState>("state");

		if (!entity || !requiredState)
			return BT::NodeStatus::FAILURE;

		const AIState &actualState = ecs.getComponent<AI>(entity.value()).state;

		if (actualState != requiredState.value())
			return BT::NodeStatus::FAILURE;

		return BT::NodeStatus::SUCCESS;
	}

  private:
	Easys::ECS &ecs;
};