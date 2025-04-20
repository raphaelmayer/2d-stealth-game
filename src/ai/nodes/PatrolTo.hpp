#pragma once

#include "../../components/Pathfinding.hpp"
#include "../../components/Patrol.hpp"
#include "../../components/Positionable.hpp"
#include "../../constants.hpp"
#include "../../engine/types/Vec2f.hpp"
#include "../../modules/AStar.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <easys/easys.hpp>
#include <string>

class PatrolTo : public BT::SyncActionNode {
  public:
	PatrolTo(const std::string &name, const BT::NodeConfig &config, Easys::ECS &ecs_)
	    : BT::SyncActionNode(name, config), ecs(ecs_)
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Easys::Entity>("entity"),
			BT::OutputPort<Vec2f>("targetPosition"),
			BT::OutputPort<Rotation>("direction"),
			BT::OutputPort<double>("duration")
		};
		// clang-format on
	}

	BT::NodeStatus tick() override
	{
		BT::Expected<Easys::Entity> entity = getInput<Easys::Entity>("entity");

		if (!entity)
			return BT::NodeStatus::FAILURE;

		if (!ecs.hasComponent<Patrol>(entity.value()))
			return BT::NodeStatus::FAILURE;

		const Vec2f &position = ecs.getComponent<Positionable>(entity.value()).position;
		const auto &targetPosition = ecs.getComponent<Pathfinding>(entity.value()).targetPosition;
		auto &patrol = ecs.getComponent<Patrol>(entity.value());
		const PatrolPoint &currentPatrolPoint = patrol.waypoints[patrol.patrolIndex];

		// Reached patrol point, set next one.
		if (position == Utils::toFloat(targetPosition) && targetPosition == currentPatrolPoint.position
		    || targetPosition == Vec2i{-1, -1}) {

			patrol.patrolIndex = (patrol.patrolIndex + 1) % patrol.waypoints.size();
			const PatrolPoint newPatrolPoint = patrol.waypoints[patrol.patrolIndex];

			setOutput<Vec2f>("targetPosition", Utils::toFloat(newPatrolPoint.position));
			setOutput<Rotation>("direction", newPatrolPoint.rotation);
			setOutput<double>("duration", newPatrolPoint.duration);
		}

		return BT::NodeStatus::SUCCESS;
	}

  private:
	Easys::ECS &ecs;
};