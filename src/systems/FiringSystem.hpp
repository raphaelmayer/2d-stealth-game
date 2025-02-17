#include "../components/EquippedWeapon.hpp"
#include "../components/Positionable.hpp"
#include "../components/Target.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../entities/projectile.hpp"
#include "../modules/Camera.hpp"
#include "../modules/StateMachine.hpp"
#include "System.hpp"
#include <set>

class FiringSystem final : public System {
  public:
	explicit FiringSystem(const Engine &engine, const Camera &camera) : engine_(engine), camera_(camera) {}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = ecs.getEntities(); // no need to iterate over all entities
		Vec2i mousePos = engine_.getMousePosition();

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<EquippedWeapon>(entity) && ecs.hasComponent<Positionable>(entity)) {
				auto &ew = ecs.getComponent<EquippedWeapon>(entity);
				auto &position = ecs.getComponent<Positionable>(entity).position;

				handleFiring(ecs, entity, deltaTime);
			}
		}
	}

  private:
	const Engine &engine_;
	const Camera &camera_;

	// we either need to store the SM within a component or we use a dedicated SMManager and just use entitiy ids to
	// index the correct SM, like we are doing with e.g. BTManager.
	// StateMachine firingSM{std::make_unique<IdleNode>()};
	// std::unordered_map<Entity, StateMachine> stateMachines;

	void handleFiring(ECSManager &ecs, const Entity &entity, const double deltaTime)
	{
		EquippedWeapon &ew = ecs.getComponent<EquippedWeapon>(entity);
		WeaponMetadata wdata = WeaponDatabase::getInstance().get(ew.weaponId);

		if (ew.firerateAccumulator > wdata.firerate) {
			ew.firerateAccumulator = 0.f;
		}

		if (ew.firerateAccumulator > 0.f) {
			ew.firerateAccumulator += static_cast<float>(deltaTime);
		}

		if (ew.reloadTimeAccumulator >= wdata.reloadTime) {
			ew.magazineSize = wdata.magazineSize;
			ew.reloadTimeAccumulator = 0.f;
		}

		if (ew.reloadTimeAccumulator > 0.f) {
			ew.reloadTimeAccumulator += static_cast<float>(deltaTime);
			return;
		}

		if (!ecs.hasComponent<Target>(entity)) {
			ew.warmupAccumulator = 0.f;
		}

		bool isMoving = ecs.getComponent<RigidBody>(entity).isMoving;
		if (ecs.hasComponent<Target>(entity) && !isMoving) {
			Target targetComp = ecs.getComponent<Target>(entity);

			// TODO: check weapon range and LOS. but probably somewhere else.

			if (ew.magazineSize == 0) {
				ew.reloadTimeAccumulator += static_cast<float>(deltaTime);
				return;
			}

			if (ew.firerateAccumulator != 0.f) {
				return;
			}

			if (ew.warmupAccumulator < wdata.warmup) {
				ew.warmupAccumulator += static_cast<float>(deltaTime);
				return;
			}

			// shoot
			ew.firerateAccumulator += static_cast<float>(deltaTime);
			--ew.magazineSize;

			Vec2f start = ecs.getComponent<Positionable>(entity).position;
			Vec2f targetPos = ecs.getComponent<Positionable>(targetComp.entity).position + (TILE_SIZE / 2);
			spawnProjectile(ecs, start, targetPos, entity, ew.weaponId);
		}
	}
};
