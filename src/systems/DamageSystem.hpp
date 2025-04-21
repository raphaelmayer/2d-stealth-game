#include "../components/DamageBuffer.hpp"
#include "../components/Health.hpp"
#include "System.hpp"
#include <easys/easys.hpp>

class DamageSystem : System {
  public:
	DamageSystem() = default;

	void update(Easys::ECS &ecs, double deltaTime)
	{
		auto entities = ecs.getEntities();
		for (const auto &entity : entities) {
			if (!ecs.hasComponent<Health>(entity) || !ecs.hasComponent<DamageBuffer>(entity)) {
				continue;
			}

			const DamageBuffer &dmgBuffer = ecs.getComponent<DamageBuffer>(entity);
			Health &health = ecs.getComponent<Health>(entity);

			for (const DamageEvent &de : dmgBuffer.damageEvents) {
				health.health = std::max(0, health.health - de.amount);

				if (health.health <= 0) {
					ecs.addComponent<Tombstone>(entity, {});
				}
			}

			ecs.removeComponent<DamageBuffer>(entity);
		}
	}

  private:
};