#pragma once

#include "../components/AI.hpp"
#include "../components/Positionable.hpp"
#include "../components/Rotatable.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Vec2d.hpp"
#include "../modules/DDA.hpp"
#include "../modules/MapManager.hpp"
#include "../modules/TileRegistry.hpp" // TileMetadata struct
#include "../systems/System.hpp"
#include <cmath>
#include <iostream>

// This system is a subsystem of AISystem. This means it is contained and run within the AISystem class.
class AIPerceptionSystem : public System {
  public:
	AIPerceptionSystem(const MapManager &mapManager) : mapManager_(mapManager) { visionMap = mapManager.getWalkableMapView(); }

	void update(ECSManager &ecs, const double deltaTime)
	{
		for (const Entity &entity : ecs.getEntities()) {
			if (ecs.hasComponent<Vision>(entity)) {
				const auto &pos = ecs.getComponent<Positionable>(entity).position;
				const auto &rot = ecs.getComponent<Rotatable>(entity).rotation;
				auto &vision = ecs.getComponent<Vision>(entity);

				vision.visibleEntities.clear();

				// update vision
				for (const Entity &otherEntity : {PLAYER}) { // AI currently only needs to see the player
					const auto &otherPos = ecs.getComponent<Positionable>(otherEntity).position;

					if (isWithinViewCone(pos, otherPos, rot, vision.range, vision.angle)) {
						// Perform obstacle check
						bool didCollide = DDA::castRay(visionMap, pos.toTileSize(), otherPos.toTileSize());
						if (!didCollide) {
							vision.visibleEntities.push_back(otherEntity);
						}
					}
				}
			}

			// update hearing

			// update other sensory inputs (danger)
		}
	}

  private:

	int calculateDistance(Vec2d start, Vec2d end)
	{
		Vec2d dirVector = end - start;
		return sqrt(dirVector.x * dirVector.x + dirVector.y * dirVector.y);
	}

	// Function to calculate whether an entity is within the view cone
	bool isWithinViewCone(const Vec2d &sourcePos, const Vec2d &targetPos, Rotation rotation, float visionRange,
	                      float visionAngle) const
	{
		// Map Rotation to forward direction vectors
		Vf2d forwardDirection = rotationToVf2d(rotation);

		// Calculate vector to the target
		Vf2d toEntity = {(float)(targetPos.x - sourcePos.x), (float)(targetPos.y - sourcePos.y)};
		float distance = std::sqrt(toEntity.x * toEntity.x + toEntity.y * toEntity.y);

		// Check distance
		if (distance > visionRange)
			return false;

		// Normalize vectors
		forwardDirection = forwardDirection.norm();
		toEntity = toEntity.norm();

		// Calculate angle using dot product
		float dotProduct = forwardDirection.x * toEntity.x + forwardDirection.y * toEntity.y;
		float angleToEntity = std::acos(dotProduct) * (180.0f / M_PI); // Convert to degrees

		// Check if within vision angle
		return angleToEntity <= (visionAngle / 2);
	}

	Vf2d rotationToVf2d(Rotation rotation) const
	{

		switch (rotation) {
		case NORTH:
			return {0, -1};
			break;
		case EAST:
			return {1, 0};
			break;
		case SOUTH:
			return {0, 1};
			break;
		case WEST:
			return {-1, 0};
			break;
		}
		return {0, 0};
	}

	const MapManager &mapManager_;
	std::vector<std::vector<int>> visionMap;
};