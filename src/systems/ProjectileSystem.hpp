#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "System.hpp"
#include <set>

class ProjectileSystem final : public System {
  public:
	ProjectileSystem() {}

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
					std::cout << "Collision\n";
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
	bool wouldCollide(ECSManager &ecs, const Entity entity, const Vec2f &position)
	{
		Entity shooter = ecs.getComponent<Projectile>(entity).shooter;
		// check collision with entities
		for (const auto &otherEntity : ecs.getEntities()) {
			if (entity == otherEntity) {
				continue;
			}

			if (otherEntity == shooter) {
				continue;
			}

			if (ecs.hasComponent<Collider>(otherEntity)) {
				Vec2f otherPosition = ecs.getComponent<Positionable>(otherEntity).position;

				// TODO: read entity size from component
				const Rectf projectileBoundingBox{position.x, position.y, 3, 3};
				const Rectf otherBoundingBox{otherPosition.x, otherPosition.y, TILE_SIZE, TILE_SIZE};

				if (aabb_checkCollision(projectileBoundingBox, otherBoundingBox)) {
					return true;
				}
			}
		}

		// check collision with map
		// TODO

		return false;
	}

	bool aabb_checkCollision(const Rectf &a, const Rectf &b) const
	{
		// If one rectangle is to the left of the other
		if (a.x + a.w < b.x || b.x + b.w < a.x)
			return false;

		// If one rectangle is above the other
		if (a.y + a.h < b.y || b.y + b.h < a.y)
			return false;

		// Otherwise, there is a collision
		return true;
	}

	std::vector<Entity> toRemove;
};
