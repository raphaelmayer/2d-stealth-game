#include "../components/DamageBuffer.hpp"
#include "../components/Health.hpp"
#include "../ecs/ECSManager.hpp"
#include "System.hpp"

class DamageSystem : System {
  public:
	DamageSystem() = default;

	void update(ECSManager &ecs, double deltaTime)
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
				// TODO: add tombstone or remove components or something
				ecs.removeComponent<AI>(entity);
				ecs.removeComponent<Controllable>(entity);
			}

			ecs.removeComponent<DamageBuffer>(entity);
		}
	}

  private:
};