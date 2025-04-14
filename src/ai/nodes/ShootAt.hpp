#pragma once

#include "../../components/EquippedWeapon.hpp"
#include "../../components/Pathfinding.hpp"
#include "../../components/Positionable.hpp"
#include "../../components/RigidBody.hpp"
#include "../../components/Target.hpp"
#include "../../components/Tombstone.hpp"
#include "../../ecs/ECSManager.hpp"
#include "../../engine/types/Vec2f.hpp"
#include "../../items/WeaponDatabase.hpp"
#include "../../items/WeaponMetadata.hpp"
#include "../../modules/AStar.hpp"
#include "behaviortree_cpp/action_node.h"
#include "behaviortree_cpp/basic_types.h" // ports etc
#include "behaviortree_cpp/tree_node.h"   // NodeConfig
#include <iostream>
#include <string>

class ShootAt : public BT::StatefulActionNode {
  public:
	ShootAt(const std::string &name, const BT::NodeConfig &config, ECSManager &ecs_)
	    : BT::StatefulActionNode(name, config), ecs(ecs_), wdb(WeaponDatabase::getInstance())
	{
	}

	static BT::PortsList providedPorts()
	{
		// clang-format off
		return {
			BT::InputPort<Entity>("entity"),
			BT::InputPort<Entity>("otherEntity")
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
		BT::Expected<Entity> otherEntity = getInput<Entity>("otherEntity");

		if (!entity || !otherEntity) {
			return BT::NodeStatus::FAILURE;
		}

		if (!isInWeaponRange(ecs, entity.value(), otherEntity.value())) {
			// TODO: remove Target comp? currently the idea is that we just reject and let the next node try to handle
			// it.
			return BT::NodeStatus::FAILURE;
		}

		if (ecs.hasComponent<Tombstone>(otherEntity.value())) {
			return BT::NodeStatus::SUCCESS;
		}

		ecs.addComponent<Target>(entity.value(), Target{otherEntity.value()});
		return BT::NodeStatus::RUNNING;
	}

	void onHalted() override
	{
		// nothing to do here...
		std::cout << "MoveTo node interrupted" << std::endl;
	}

  private:
	ECSManager &ecs;
	WeaponDatabase &wdb;

	// we could think about providing a new module EntityUtils and add this function to it.
	float calculateDistance(ECSManager &ecs, const Entity entity, const Entity otherEntity)
	{
		// technically we would need to check if entities even have a position.
		// especially if we were to make it part of a generic utility module.
		auto pos1 = ecs.getComponent<Positionable>(entity).position;
		auto pos2 = ecs.getComponent<Positionable>(otherEntity).position;
		return (pos2 - pos1).length();
	}

	bool isInWeaponRange(ECSManager &ecs, const Entity entity, const Entity otherEntity)
	{
		const auto id = ecs.getComponent<EquippedWeapon>(entity).weaponId;
		const WeaponMetadata wdata = wdb.get(id);
		float distanceBetweenEntities = calculateDistance(ecs, entity, otherEntity);

		return distanceBetweenEntities < wdata.range * TILE_SIZE;
	}
};