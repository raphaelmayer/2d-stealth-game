#include "../components/Pathfinding.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../constants.hpp"
#include "../ecs/Entity.hpp"
#include "../map/MapManager.hpp"
#include "System.hpp"
#include <cmath>
#include <set>

class PhysicsSystem final : public System {
  public:
	PhysicsSystem(const MapManager &mapManager) : mapManager_(mapManager) {}

	void update(ECSManager &ecs, double deltaTime) override
	{
		const std::set<Entity> &entities = ecs.getEntities();

		for (const Entity &entity : entities) {
			if (!ecs.hasComponent<RigidBody>(entity) || !ecs.hasComponent<Positionable>(entity)) {
				continue;
			}

			auto &rigidBody = ecs.getComponent<RigidBody>(entity);
			auto &currentPosition = ecs.getComponent<Positionable>(entity).position; // in pixels (float)

			// If this entity is not trying to move anywhere, skip.
			// if (!rigidBody.isMoving) {
			if (Utils::toFloat(rigidBody.endPosition) == currentPosition) {
				continue;
			}

			rigidBody.isMoving = true; // set flag for other systems
			Vec2f endPosPx = Utils::toFloat(rigidBody.endPosition);
			Vec2f startPosPx = Utils::toFloat(rigidBody.startPosition);
			Vec2f toTarget = endPosPx - currentPosition;
			float distToTarget = toTarget.length();

			// If we are "close enough" to the target, just clamp and reset.
			if (distToTarget < 0.01f) {
				currentPosition = endPosPx;
				resetCurrentMovementParams(rigidBody, currentPosition);
				continue;
			}

			Vec2f direction = toTarget.norm();
			float maxMoveThisFrame = WALK_SPEED * static_cast<float>(deltaTime);
			// If the required distance is less than what we plan to move, clamp so that we land exactly on the tile.
			float moveDist = std::min(distToTarget, maxMoveThisFrame);
			Vec2f newPosition = currentPosition + direction * moveDist; // Tentatively compute new position

			// TODO?: We currently do a next tile check with endPosition. We might want to switch to a bounding box
			// approach, where we use newPosition to check, if the move is valid and only then apply it.
			if (wouldCollide(ecs, entity, rigidBody)) {
				// Clear path if Pathfinding, or do something else:
				if (ecs.hasComponent<Pathfinding>(entity)) {
					ecs.getComponent<Pathfinding>(entity).path.clear();
				}
				resetCurrentMovementParams(rigidBody, currentPosition);
				continue;
			}

			currentPosition = newPosition; // Update actual position if valid.

			if (ecs.hasComponent<Rotatable>(entity)) {
				applyRotation(direction, ecs.getComponent<Rotatable>(entity));
			}

			// If we’ve reached the end tile, finalize. Currently redundant check?
			distToTarget = (endPosPx - currentPosition).length();
			if (distToTarget < 0.01f) {
				currentPosition = endPosPx;
				resetCurrentMovementParams(rigidBody, currentPosition);
			}
		}
	}

  private:
	void resetCurrentMovementParams(RigidBody &rigidBody, const Vec2f &currentPosPx)
	{
		rigidBody.isMoving = false;

		Vec2i newPosition = Utils::toInt(Utils::round(currentPosPx / TILE_SIZE)) * TILE_SIZE;

		rigidBody.startPosition = newPosition;
		rigidBody.endPosition = newPosition;
	}

	// TODO: reduce potential O(N²) collision checks
	bool wouldCollide(ECSManager &ecs, Entity entity, RigidBody &rigidBody)
	{
		const Vec2i tileSizedEndPos = Utils::toTileSize(rigidBody.endPosition);
		// const Tile endTile = mapManager_.getTile(tileSizedEndPos.x, tileSizedEndPos.y);

		if (ecs.hasComponent<Collider>(entity)) {
			// check if any collidable entity occupies the tile we are trying to move on
			for (const Entity &other : ecs.getEntities()) {
				if (entity != other && ecs.hasComponent<Collider>(other) && ecs.hasComponent<Positionable>(other)) {
					const auto &otherPosition = ecs.getComponent<Positionable>(other).position;
					if (Utils::toInt(Utils::round(otherPosition)) == rigidBody.endPosition) {
						return true;
					}
					// Entities might move onto the same tile at the same time.
					if (ecs.hasComponent<RigidBody>(other)) {
						const auto &otherEndPosition = ecs.getComponent<RigidBody>(other).endPosition;
						if (otherEndPosition == rigidBody.endPosition) {
							return true;
						}
					}
				}
			}

			// check map tiles separately, because they are not entities
			// TODO: improve collision detection between player and map, as soon as map loading is sorted
			return mapManager_.getWalkableMapView()[tileSizedEndPos.y][tileSizedEndPos.x];
			// if (endTile.objectId == 0) // id 0 implies no/empty tile. temporary fix.
			//	return !mapManager_.getTileData(endTile.backgroundId).walkable;
			// else
			//	return !mapManager_.getTileData(endTile.objectId).walkable;

			// std::cout << mapManager_.getTileData(endTile.objectId).walkable << "\n";
		}

		return false;
	}

	void applyRotation(const Vec2f &moveDir, Rotatable &rotatable) const
	{
		// Because we used "toTarget.norm()" it could have any angle,
		// but we only handle 4 directions, so pick the largest axis.
		if (std::fabs(moveDir.x) > std::fabs(moveDir.y)) {
			// Horizontal movement
			if (moveDir.x > 0.f) {
				rotatable.rotation = Rotation::EAST;
			} else {
				rotatable.rotation = Rotation::WEST;
			}
		} else {
			// Vertical movement
			if (moveDir.y > 0.f) {
				rotatable.rotation = Rotation::SOUTH;
			} else {
				rotatable.rotation = Rotation::NORTH;
			}
		}
	}

  private:
	const MapManager &mapManager_;
};
