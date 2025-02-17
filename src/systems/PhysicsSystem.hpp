#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../constants.hpp"
#include "../ecs/Entity.hpp"
#include "../modules/MapManager.hpp"
#include "System.hpp"
#include <cmath>
#include <set>

class PhysicsSystem final : public System {
  public:
	PhysicsSystem(const MapManager &mapManager) : mapManager_(mapManager) {}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = ecs.getEntities();

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<RigidBody>(entity) && ecs.hasComponent<Positionable>(entity)) {
				auto &position = ecs.getComponent<Positionable>(entity).position;
				auto &rigidBody = ecs.getComponent<RigidBody>(entity);

				// if (rigidBody.isMoving) {
				if (rigidBody.endPosition != position) {
					rigidBody.isMoving = true;
					if (ecs.hasComponent<Rotatable>(entity))
						applyRotation(position, rigidBody, ecs.getComponent<Rotatable>(entity));
					if (checkCollisions(ecs, entity, rigidBody)) {
						resetCurrentMovementParams(position, rigidBody);
						// If path becomes blocked, we need to recalculate.
						if (ecs.hasComponent<AI>(entity))
							ecs.getComponent<AI>(entity).path = {};
					}
					// while progress is smaller than TILE_SIZE, we move WALKSPEED pixels per second in this direction
					if (rigidBody.progress < TILE_SIZE) {
						applyMovement(position, rigidBody, deltaTime);
						if (position == rigidBody.endPosition)
							resetCurrentMovementParams(position, rigidBody);
					} else {
						// we might slightly overshoot endPos, thus we clamp position back to the grid
						position = rigidBody.endPosition;
						resetCurrentMovementParams(position, rigidBody);
					}
				} else {
					resetCurrentMovementParams(position, rigidBody);
				}
			}
		}
	}

  private:
	void resetCurrentMovementParams(const Vec2i &position, RigidBody &rigidBody)
	{
		rigidBody.isMoving = false;
		rigidBody.progress = 0;

		// the following operations only a safety net for grid clamping, if something were to go wrong.
		rigidBody.startPosition = position.toTileSize() * TILE_SIZE;
		rigidBody.endPosition = position.toTileSize() * TILE_SIZE;
	}

	bool checkCollisions(ECSManager &ecs, Entity entity, RigidBody &rigidBody)
	{
		const Vec2i tileSizedEndPos = rigidBody.endPosition.toTileSize();
		const Tile endTile = mapManager_.getTile(tileSizedEndPos.x, tileSizedEndPos.y);

		if (ecs.hasComponent<Collider>(entity)) {
			// check if any collidable entity occupies the tile we are trying to move on
			for (const Entity &other : ecs.getEntities()) {
				if (entity != other && ecs.hasComponent<Collider>(other) && ecs.hasComponent<Positionable>(other)) {
					const auto &otherPosition = ecs.getComponent<Positionable>(other).position;
					if (otherPosition == rigidBody.endPosition) {
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
			// if (mapManager_.getWalkableMapView()[tileSizedEndPos.y][tileSizedEndPos.x])
			if (!mapManager_.getTileData(endTile.objectId).walkable)
				return true;
		}

		return false;
	}

	void applyMovement(Vec2i &position, RigidBody &rigidBody, double deltaTime)
	{
		const float movementAmount = WALK_SPEED * (float)deltaTime;
		rigidBody.accumulator += movementAmount;
		rigidBody.progress += movementAmount;

		const Vec2i direction = (rigidBody.endPosition - position).sign();

		// This is not optimal and can result in jittery movements, when the framerate is low.
		// This is due to float calculations.
		if (rigidBody.accumulator >= 1.0) {
			position += direction * rigidBody.accumulator;
			rigidBody.accumulator -= static_cast<int>(rigidBody.accumulator);
		}
	}

	void applyRotation(Vec2i &position, RigidBody &rigidBody, Rotatable &rotatable)
	{
		const Vec2i direction = (rigidBody.endPosition - position).sign();

		if (direction.y == -1) {
			rotatable.rotation = Rotation::NORTH;
		} else if (direction.x == 1) {
			rotatable.rotation = Rotation::EAST;
		} else if (direction.y == 1) {
			rotatable.rotation = Rotation::SOUTH;
		} else if (direction.x == -1) {
			rotatable.rotation = Rotation::WEST;
		}
	}

	MapManager mapManager_;
};
