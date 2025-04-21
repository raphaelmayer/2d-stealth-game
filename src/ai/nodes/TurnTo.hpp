#pragma once

#include "../../components/Positionable.hpp"
#include "../../components/Rotatable.hpp"
#include "../../components/Vision.hpp"
#include "../../constants.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <easys/easys.hpp>
#include <string>

class TurnTo : public BT::SyncActionNode {
  public:
	TurnTo(const std::string &name, const BT::NodeConfig &config, Easys::ECS &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Easys::Entity>("entity"),
			BT::InputPort<Rotation>("direction")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Easys::Entity> entity = getInput<Easys::Entity>("entity");
		BT::Expected<Rotation> direction = getInput<Rotation>("direction");

		if (!entity)
			return BT::NodeStatus::FAILURE;

		auto &rotation = ecs.getComponent<Rotatable>(entity.value()).rotation;
		rotation = direction.value();

		return BT::NodeStatus::SUCCESS;
	}

  private:
	Easys::ECS &ecs;
};