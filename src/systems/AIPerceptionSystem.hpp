#pragma once

#include "../components/AI.hpp"
#include "../components/Positionable.hpp"
#include "../components/Rotatable.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../map/MapManager.hpp"
#include "../map/TileRegistry.hpp" // TileMetadata struct
#include "../modules/DDA.hpp"
#include "../modules/Utils.hpp"
#include "../systems/System.hpp"
#include <cmath>
#include <iostream>

// This system is a subsystem of AISystem. This means it is contained and run within the AISystem class.
class AIPerceptionSystem : public System {
  public:
	AIPerceptionSystem(const MapManager &mapManager) : mapManager_(mapManager)
	{
		visionMap = mapManager.getWalkableMapView();
	}

	void update(ECSManager &ecs, const double deltaTime)
	{
		for (const Entity &entity : ecs.getEntities()) {
			if (ecs.hasComponent<Vision>(entity)) {
				const auto &pos = ecs.getComponent<Positionable>(entity).position;
				const auto &rot = ecs.getComponent<Rotatable>(entity).rotation;
				auto &vision = ecs.getComponent<Vision>(entity);

				vision.visibleEnemies.clear();
				vision.visibleAllies.clear();

				// update vision
				for (const Entity &otherEntity : ecs.getEntities()) { // AI currently only needs to see the player
					if (entity == otherEntity)
						continue;

					const auto &otherPos = ecs.getComponent<Positionable>(otherEntity).position;

					if (isWithinViewCone(pos, otherPos, rot, (float)vision.range, (float)vision.angle)) {
						// Perform obstacle check
						bool didCollide = DDA::castRay(visionMap, Utils::toTileSize(pos), Utils::toTileSize(otherPos));
						if (!didCollide) {
							if (otherEntity == PLAYER) // Currently only player can be enemy. No notion of factions etc.
								vision.visibleEnemies.push_back(otherEntity);
							else
								vision.visibleAllies.push_back(otherEntity);
						}
					}
				}
			}

			// update hearing

			// update other sensory inputs (danger)
		}
	}

  private:
	// Function to calculate whether an entity is within the view cone
	bool isWithinViewCone(const Vec2f &sourcePos, const Vec2f &targetPos, Rotation rotation, float visionRange,
	                      float visionAngle) const
	{
		Vec2f forwardDirection = Utils::rotationToVec2f(rotation);
		Vec2f toEntity = {(float)(targetPos.x - sourcePos.x), (float)(targetPos.y - sourcePos.y)};
		const float distance = toEntity.length();

		if (distance > visionRange)
			return false;

		forwardDirection = forwardDirection.norm();
		toEntity = toEntity.norm();

		// Calculate angle using dot product
		float dotProduct = forwardDirection.x * toEntity.x + forwardDirection.y * toEntity.y;
		float angleToEntity = std::acos(dotProduct) * (180.0f / (float)M_PI); // Convert to degrees

		// Check if within vision angle
		return angleToEntity <= (visionAngle / 2);
	}

	const MapManager &mapManager_;
	std::vector<std::vector<int>> visionMap;
};