#include "../components/Pathfinding.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../constants.hpp"
#include "../ecs/Entity.hpp"
#include "../map/MapManager.hpp"
#include "System.hpp"
#include <cmath>
#include <set>

/*
 * This system manages entity movement, collision detection, and rotation updates, ensuring accurate position
 * transitions and resolving path obstructions. To improve modularity, it could be split into separate services for
 * movement handling, collision detection, and rotation adjustments.
 *
 * TODO: Consider splitting this system into dedicated services (e.g., MovementService, CollisionService, and
 *		 RotationService) to improve separation of concerns and maintainability.
 */

class PhysicsSystem final : public System {
  public:
	PhysicsSystem(const MapManager &mapManager) : mapManager_(mapManager)
	{
	}

	void update(ECSManager &ecs, double deltaTime) override
	{
		const std::set<Entity> &entities = ecs.getEntities();

		for (const Entity &entity : entities) {
			if (!ecs.hasComponent<RigidBody>(entity) || !ecs.hasComponent<Positionable>(entity)) {
				continue;
			}

			auto &rigidBody = ecs.getComponent<RigidBody>(entity);
			auto &currentPos = ecs.getComponent<Positionable>(entity).position; // in pixels (float)
			Vec2f nextPos = Utils::toFloat(rigidBody.nextPosition);

			if (isTargetReached(currentPos, nextPos)) {
				continue;
			}

			rigidBody.isMoving = true;                                         // set flag for other systems
			Movement move = calculateMovement(currentPos, nextPos, deltaTime); // Tentatively compute new position

			if (wouldCollide(ecs, entity, nextPos)) {
				if (ecs.hasComponent<Pathfinding>(entity)) {
					ecs.getComponent<Pathfinding>(entity).path.clear();
					ecs.getComponent<Collider>(entity).didCollide = true;
					ecs.getComponent<Collider>(entity).lastCollisionPosition = nextPos;

				}
				resetCurrentMovementParams(rigidBody, currentPos);
				continue;
			}

			currentPos = move.newPosition; // Update actual position if valid.
			handleRotation(ecs, entity, move.direction);

			// If we’ve reached the end tile, finalize. Redundant check, since we also do this in calculateMovement?
			float distToTarget = (nextPos - currentPos).length();
			if (distToTarget < 0.01f) {
				currentPos = nextPos;
				resetCurrentMovementParams(rigidBody, currentPos);
			}
		}
	}

  private:
	struct Movement {
		Vec2f newPosition;
		Vec2f direction;
		float distance;
	};

	Movement calculateMovement(const Vec2f &currentPos, const Vec2f &nextPos, double deltaTime) const
	{
		Vec2f toTarget = nextPos - currentPos;
		float distToTarget = toTarget.length();

		// If we are "close enough" to the target, just clamp and reset.
		if (distToTarget < 0.01f) {
			return {nextPos, {0.0f, 0.0f}, 0.0f};
		}

		Vec2f direction = toTarget.norm();
		float maxMoveThisFrame = WALK_SPEED * static_cast<float>(deltaTime);
		// If the required distance is less than what we plan to move, clamp so that we land exactly on the tile.
		float moveDist = std::min(distToTarget, maxMoveThisFrame);
		Vec2f newPosition = currentPos + direction * moveDist; // Tentatively compute new position

		return {newPosition, direction, distToTarget};
	}

	bool isTargetReached(const Vec2f &currentPos, const Vec2f &nextPos) const
	{
		return nextPos == currentPos;
	}

	void resetCurrentMovementParams(RigidBody &rigidBody, const Vec2f &currentPos) const
	{
		rigidBody.isMoving = false;
		Vec2i newPosition = Utils::toInt(Utils::round(currentPos / TILE_SIZE)) * TILE_SIZE;
		rigidBody.startPosition = newPosition;
		rigidBody.nextPosition = newPosition;
	}

	bool wouldCollide(ECSManager &ecs, Entity entity, const Vec2f &nextPos) const
	{
		const Vec2i tileSizedEndPos = Utils::toTileSize(nextPos);

		if (ecs.hasComponent<Collider>(entity)) {
			// check map tiles separately, because they are not entities
			if (mapManager_.getWalkableMapView()[tileSizedEndPos.y][tileSizedEndPos.x]) {
				return true;
			}

			if (wouldCollideWithEntity(ecs, entity, nextPos)) {
				return true;
			}
		}

		return false;
	}

	// Check if any collidable entity occupies the tile we are trying to move on.
	// TODO?: We currently do a next tile check with nextPosition. We might want to switch to a bounding box
	// approach, where we use newPosition to check, if the move is valid and only then apply it.
	bool wouldCollideWithEntity(ECSManager &ecs, Entity entity, const Vec2f &nextPos) const
	{
		// TODO: reduce potential O(N²) collision checks
		for (const Entity &other : ecs.getEntities()) {
			if (entity != other && ecs.hasComponent<Collider>(other) && ecs.hasComponent<Positionable>(other)) {
				const auto &otherPosition = ecs.getComponent<Positionable>(other).position;
				if (Utils::round(otherPosition) == nextPos) {
					return true;
				}
				// Entities might move onto the same tile at the same time.
				if (ecs.hasComponent<RigidBody>(other)) {
					const Vec2i &otherEndPosition = ecs.getComponent<RigidBody>(other).nextPosition;
					if (Utils::toFloat(otherEndPosition) == nextPos) {
						return true;
					}
				}
			}
		}

		return false;
	}

	void handleRotation(ECSManager &ecs, const Entity entity, const Vec2f &direction) const
	{
		if (ecs.hasComponent<Rotatable>(entity)) {
			auto &rotatable = ecs.getComponent<Rotatable>(entity);
			rotatable.rotation = Utils::vec2fToRotation(direction);
		}
	}

	const MapManager &mapManager_;
};
