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
				Projectile &projectile = ecs.getComponent<Projectile>(entity);
				Vec2f targetPosition = projectile.targetPosition;
				float velocity = projectile.velocity;
				Vec2f toTarget = targetPosition - position;

				Vec2f startToTarget = targetPosition - projectile.startPosition;
				Vec2f startToPosition = position - projectile.startPosition;

				// A projectile could have either a target position or an exit angle

				if (toTarget.length() < 0.01 || (startToTarget.length() - startToPosition.length()) < 0.0f) {
					position = targetPosition;
					toRemove.push_back(entity);
				} else {
					position += (toTarget.norm() * velocity * deltaTime);
				}
			}
		}

		for (auto &entity : toRemove) {
			ecs.removeEntity(entity);
		}
		toRemove.clear();
	}

  private:
	std::vector<Entity> toRemove;
};
