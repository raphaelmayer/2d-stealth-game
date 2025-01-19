#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../ecs/ECSManager.hpp"
#include "System.hpp"
#include <set>

class ProjectileSystem final : public System {
  public:
	ProjectileSystem() {}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = ecs.getEntities();
		Vec2i targetPosition{0, 0};
		Vec2i toTarget = targetPosition - position;
		int projectileSpeed = 50;

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Projectile>(entity) && ecs.hasComponent<Positionable>(entity)) {
				auto &position = ecs.getComponent<Positionable>(entity);
				auto &projectile = ecs.getComponent<Projectile>(entity);

				// TODO: A projectile could have either a target position or an exit angle

				if (position == targetPosition) {
					ecs.removeEntity();
				} else {
					position += projectileSpeed * deltaTime;
				}
			}
		}

	  private:
	};
