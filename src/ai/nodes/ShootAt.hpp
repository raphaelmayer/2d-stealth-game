#pragma once

#include "../../components/EquippedWeapon.hpp"
#include "../../components/Pathfinding.hpp"
#include "../../components/Positionable.hpp"
#include "../../components/RigidBody.hpp"
#include "../../components/Target.hpp"
#include "../../components/Tombstone.hpp"
#include "../../engine/types/Vec2f.hpp"
#include "../../items/WeaponDatabase.hpp"
#include "../../items/WeaponMetadata.hpp"
#include "../../modules/AStar.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <easys/easys.hpp>
#include <iostream>
#include <string>

class ShootAt : public BT::StatefulActionNode {
  public:
	ShootAt(const std::string &name, const BT::NodeConfig &config, Easys::ECS &ecs_)
	    : BT::StatefulActionNode(name, config), ecs(ecs_), wdb(WeaponDatabase::getInstance())
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Easys::Entity>("entity"),
			BT::InputPort<Easys::Entity>("otherEntity"),
			BT::OutputPort<Vec2f>("position")
		};
		// clang-format on
	}

	BT::NodeStatus onStart() override
	{
		BT::Expected<Easys::Entity> expEntity = getInput<Easys::Entity>("entity");
		BT::Expected<Easys::Entity> expOtherEntity = getInput<Easys::Entity>("otherEntity");

		if (!expEntity || !expOtherEntity) {
			return BT::NodeStatus::FAILURE;
		}

		entity = expEntity.value();
		otherEntity = expOtherEntity.value();

		return BT::NodeStatus::RUNNING;
	}

	BT::NodeStatus onRunning() override
	{
		if (!ecs.hasEntity(otherEntity)) {
			return BT::NodeStatus::FAILURE;
		}
		
		// this check could be its own node.
		if (!isInWeaponRange(ecs, entity, otherEntity)) {
			// TODO: remove Target comp? currently we just reject and let the next node try to handle it.
			//
			// Currently we read the other position from the IsEnemyVisible node.
			// const Vec2f otherPosition = ecs.getComponent<Positionable>(otherEntity).position;
			// setOutput<Vec2f>("position", otherPosition);
			return BT::NodeStatus::FAILURE;
		}

		if (ecs.hasComponent<Tombstone>(otherEntity)) {
			return BT::NodeStatus::SUCCESS;
		}

		ecs.addComponent<Target>(entity, Target{otherEntity});

		// Is this a good choice to do here? currently done in firingsystem
		// ecs.addComponent<Pathfinding>(entity, {}); // clear path

		return BT::NodeStatus::RUNNING;
	}

	void onHalted() override
	{
		// nothing to do here...
		std::cout << "MoveTo node interrupted" << std::endl;
	}

  private:
	Easys::ECS &ecs;
	WeaponDatabase &wdb;

	Easys::Entity entity;
	Easys::Entity otherEntity;

	// we could think about providing a new module EntityUtils and add this function to it.
	float calculateDistance(Easys::ECS &ecs, const Easys::Entity entity, const Easys::Entity otherEntity) const
	{
		// technically we would need to check if entities even have a position.
		// especially if we were to make it part of a generic utility module.
		auto pos1 = ecs.getComponent<Positionable>(entity).position;
		auto pos2 = ecs.getComponent<Positionable>(otherEntity).position;
		return (pos2 - pos1).length();
	}

	bool isInWeaponRange(Easys::ECS &ecs, const Easys::Entity entity, const Easys::Entity otherEntity) const
	{
		const auto id = ecs.getComponent<EquippedWeapon>(entity).weaponId;
		const WeaponMetadata wdata = wdb.get(id);
		const float distanceBetweenEntities = calculateDistance(ecs, entity, otherEntity);

		return distanceBetweenEntities < (wdata.range * TILE_SIZE);
	}
};