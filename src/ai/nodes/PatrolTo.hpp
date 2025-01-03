#pragma once

#include "../../components/AI.hpp"
#include "../../components/Patrol.hpp"
#include "../../components/Positionable.hpp"
#include "../../constants.hpp"
#include "../../ecs/ECSManager.hpp"
#include "../../engine/Vec2d.hpp"
#include "../../modules/AStar.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <string>

class PatrolTo : public BT::SyncActionNode {
  public:
	PatrolTo(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity"),
			BT::OutputPort<Vec2d>("targetPosition"),
			BT::OutputPort<Rotation>("direction"),
			BT::OutputPort<double>("duration")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Entity> entity = getInput<Entity>("entity");

		if (!entity)
			return BT::NodeStatus::FAILURE;

		if (!ecs.hasComponent<Patrol>(entity.value()))
			return BT::NodeStatus::FAILURE;

		auto &patrol = ecs.getComponent<Patrol>(entity.value());
		auto &ai = ecs.getComponent<AI>(entity.value());
		const auto &position = ecs.getComponent<Positionable>(entity.value()).position;
		auto currentPatrolPoint = patrol.waypoints[patrol.patrolIndex];

		// Reached patrol point, set next one.
		if (position == ai.targetPosition && ai.targetPosition == currentPatrolPoint.position
		    || ai.targetPosition == Vec2d{-1, -1}) {

			patrol.patrolIndex = (patrol.patrolIndex + 1) % patrol.waypoints.size();
			currentPatrolPoint = patrol.waypoints[patrol.patrolIndex];

			setOutput<Vec2d>("targetPosition", currentPatrolPoint.position);
			setOutput<Rotation>("direction", currentPatrolPoint.rotation);
		    setOutput<double>("duration", currentPatrolPoint.duration);
		}

		return BT::NodeStatus::SUCCESS;
	}

  private:
	ECSManager &ecs;
};