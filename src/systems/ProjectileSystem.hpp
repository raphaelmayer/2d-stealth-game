#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../components/Tombstone.hpp"
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
				const std::optional<CollisionResult> collision = wouldCollide(ecs, entity, newPosition);

				if (collision) {
					applyDamage(ecs, *collision);
					ecs.addComponent<Tombstone>(entity, Tombstone{}); // mark projectile to be removed
				}

				if (toTarget.length() < 0.01) {
					position = targetPosition;
					ecs.addComponent<Tombstone>(entity, Tombstone{});
				} else {
					position = newPosition;
				}
			}
		}
	}

  private:
	const MapManager &mapmanager_;

	struct CollisionResult {
		// bool didCollide = false; // TODO: probably need something like this, since we always generate a collision
		// event.
		//  but do we need to or could we solve this differently?
		Entity a = 0;   // entity initiating collision
		Entity b = 0;   // entity we are colliding with
		Vec2f position; // position of collision (wrt. a)
	};

	std::optional<CollisionResult> wouldCollide(ECSManager &ecs, const Entity entity, const Vec2f &position) const
	{
		if (checkCollisionsWithMap(ecs, entity, position)) {
			return CollisionResult{entity, entity, position}; // TODO: use special entity value if collision with map
		}

		return checkCollisionsWithEntities(ecs, entity, position);
	}

	bool checkCollisionsWithMap(ECSManager &ecs, const Entity entity, const Vec2f &position) const
	{
		// TODO: check collision with map
		// We need to implement a penetrable property for tiles.
		// Since we change our tileset soon, we'll defer until this is done.

		return false;
	}

	std::optional<CollisionResult> checkCollisionsWithEntities(ECSManager &ecs, const Entity entity,
	                                                           const Vec2f &position) const
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
				const int size = 3; // TODO: read entity size from component
				const Rectf projectileBoundingBox{position.x, position.y, size, size};
				const Rectf otherBoundingBox{otherPosition.x, otherPosition.y, TILE_SIZE, TILE_SIZE};

				if (AABB::checkCollision(projectileBoundingBox, otherBoundingBox)) {
					return CollisionResult{entity, otherEntity, position};
				}
			}
		}

		return std::nullopt;
	}

	void applyDamage(ECSManager &ecs, const CollisionResult &collisionResult)
	{
		Projectile prj = ecs.getComponent<Projectile>(collisionResult.a);
		int amount = prj.damage;
		Entity targetEntity = collisionResult.b;
		DamageEvent dmgEvent{collisionResult.a, amount};

		if (ecs.hasComponent<DamageBuffer>(targetEntity)) {
			DamageBuffer &dmgBuffer = ecs.getComponent<DamageBuffer>(targetEntity);
			dmgBuffer.damageEvents.push_back(dmgEvent);
		} else {
			DamageBuffer dmgBuffer;
			dmgBuffer.damageEvents.push_back(dmgEvent);
			ecs.addComponent<DamageBuffer>(targetEntity, dmgBuffer);
		}
	}
};
