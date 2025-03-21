#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../map/MapManager.hpp"
#include "../modules/AABB.hpp"
#include "System.hpp"
#include <set>

class ProjectileSystem final : public System {
  public:
	ProjectileSystem(const MapManager &mapmanager) : mapmanager_(mapmanager)
	{
	}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = ecs.getEntities();

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Projectile>(entity) && ecs.hasComponent<Positionable>(entity)) {
				Vec2f &position = ecs.getComponent<Positionable>(entity).position;
				const Projectile &projectile = ecs.getComponent<Projectile>(entity);
				const Vec2f startPosition = projectile.startPosition;
				const Vec2f targetPosition = projectile.targetPosition;
				const float velocity = projectile.velocity;

				const Vec2f toTarget = targetPosition - position;
				const float moveDistance = std::min(toTarget.length(), velocity * static_cast<float>(deltaTime));
				const Vec2f newPosition = position + (toTarget.norm() * moveDistance);

				if (wouldCollide(ecs, entity, newPosition)) {
					// TODO: apply damage or register hit and let another system handle damage
					toRemove.push_back(entity);
				}

				if (toTarget.length() < 0.01) {
					position = targetPosition;
					toRemove.push_back(entity);
				} else {
					position = newPosition;
				}
			}
		}

		for (auto &entity : toRemove) {
			ecs.removeEntity(entity);
		}
		toRemove.clear();
	}

  private:
	const MapManager &mapmanager_;
	std::vector<Entity> toRemove;

	bool wouldCollide(ECSManager &ecs, const Entity entity, const Vec2f &position) const
	{
		if (checkCollisionsWithMap(ecs, entity, position)) {
			return true;
		}

		if (checkCollisionsWithEntities(ecs, entity, position)) {
			return true;
		}

		return false;
	}

	bool checkCollisionsWithMap(ECSManager &ecs, const Entity entity, const Vec2f &position) const
	{
		// TODO: check collision with map
		// We need to implement a penetrable property for tiles.
		// Since we change our tileset soon, we'll defer until this is done.

		return false;
	}

	bool checkCollisionsWithEntities(ECSManager &ecs, const Entity entity, const Vec2f &position) const
	{
		const Entity shooter = ecs.getComponent<Projectile>(entity).shooter;

		for (const auto &otherEntity : ecs.getEntities()) {
			if (entity == otherEntity) {
				continue;
			}

			if (otherEntity == shooter) {
				continue;
			}

			if (ecs.hasComponent<Collider>(otherEntity)) {
				const Vec2f otherPosition = ecs.getComponent<Positionable>(otherEntity).position;
				// TODO: read entity size from component
				const Rectf projectileBoundingBox{position.x, position.y, 3, 3};
				const Rectf otherBoundingBox{otherPosition.x, otherPosition.y, TILE_SIZE, TILE_SIZE};

				if (AABB::checkCollision(projectileBoundingBox, otherBoundingBox)) {
					return true;
				}
			}
		}

		return false;
	}
};
