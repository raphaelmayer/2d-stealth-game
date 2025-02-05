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
		// check collision with entities
		for (const auto &otherEntity : ecs.getEntities()) {
			if (entity == otherEntity) {
				continue;
			}

			if (ecs.hasComponent<Collider>(otherEntity)) {
				Vec2f otherPosition = ecs.getComponent<Positionable>(otherEntity).position;
				// Currently we naively check positions directly, without regarding size / bounding box.
				if ((position - otherPosition).length() < 0.1f) {
					return true;
				}
			}
		}

		// check collision with map
		// TODO

		return false;
	}

	std::vector<Entity> toRemove;
};
