#pragma once

#include "../../components/Vision.hpp"
#include "../../ecs/ECSManager.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <string>

// I guess we could set a vector of all visible enemies (or just a check, since we could reuse
// visibleEntities in later nodes) and let succeeding nodes handle their specifics like who to target etc.
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
			BT::InputPort<Entity>("entity")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		Entity entity = getInputOrThrow<Entity>("entity");
		Vision vision = ecs.getComponent<Vision>(entity);

		if (vision.visibleEntities.size() > 0) {
			return BT::NodeStatus::SUCCESS;
		}

		return BT::NodeStatus::FAILURE;
	}

  private:
	// TODO: Implement a baseclass, which implements this function? Syntax is kind of neat.
	template <typename T>
	T getInputOrThrow(std::string key)
	{
		BT::Expected<T> exp = getInput<T>(key);

		if (!exp)
			throw std::runtime_error("Node input \"" + key + "\" could not be found.");

		return exp.value();
	}

	ECSManager &ecs;
};