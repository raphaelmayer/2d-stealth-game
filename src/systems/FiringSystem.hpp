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
	explicit FiringSystem(const Engine &engine) : engine_(engine)
	{
	}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const Vec2i mousePos = engine_.getMousePosition();
		const std::set<Entity> &entities = ecs.getEntities(); // no need to iterate over all entities

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<EquippedWeapon>(entity) && ecs.hasComponent<Positionable>(entity)) {
				handleFiring(ecs, entity, deltaTime);
			}
		}
	}

  private:
	const Engine &engine_;

	// we either need to store the SM within a component or we use a dedicated SMManager and just use entitiy ids to
	// index the correct SM, like we are doing with e.g. BTManager.
	// StateMachine firingSM{std::make_unique<IdleNode>()};
	// std::unordered_map<Entity, StateMachine> stateMachines;

	Vec2f calculateLead(const Vec2f &shooterPosition, const Vec2f &targetPosition, const float projectileVelocity,
	                    const Vec2f &targetVelocity) const
	{
		Vec2f relativePosition = targetPosition - shooterPosition;
		Vec2f relativeVelocity = targetVelocity; // Change if shooter can move

		float a = relativeVelocity.lengthSquared() - (projectileVelocity * projectileVelocity);
		float b = 2.f * relativePosition.dot(relativeVelocity);
		float c = relativePosition.lengthSquared();

		float discriminant = b * b - 4.f * a * c;

		// No solution case
		if (discriminant < 0) {
			return targetPosition;
		}

		float sqrtDiscriminant = std::sqrt(discriminant);
		float t1 = (-b - sqrtDiscriminant) / (2.f * a);
		float t2 = (-b + sqrtDiscriminant) / (2.f * a);

		// Find the smallest positive time
		float timeToImpact = -1.f;
		if (t1 > 0 && t2 > 0) {
			timeToImpact = std::min(t1, t2);
		} else if (t1 > 0) {
			timeToImpact = t1;
		} else if (t2 > 0) {
			timeToImpact = t2;
		}

		if (timeToImpact <= 0) {
			return targetPosition; // No valid future impact
		}

		return targetPosition + targetVelocity * timeToImpact;
	}

	void handleFiring(ECSManager &ecs, const Entity &entity, const double deltaTime) const
	{
		EquippedWeapon &ew = ecs.getComponent<EquippedWeapon>(entity);
		WeaponMetadata wdata = WeaponDatabase::getInstance().get(ew.weaponId);

		bool &isReloading = ecs.getComponent<EquippedWeapon>(entity).isReloading;

		if (ew.firerateAccumulator > wdata.firerate) {
			ew.firerateAccumulator = 0.f;
		}

		if (ew.firerateAccumulator > 0.f) {
			ew.firerateAccumulator += static_cast<float>(deltaTime);
		}

		if (ew.reloadTimeAccumulator >= wdata.reloadTime) {
			isReloading = false;
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
		bool& isShooting = ecs.getComponent<RigidBody>(entity).isShooting;
		if (ecs.hasComponent<Target>(entity) && !isMoving) {
			Target targetComp = ecs.getComponent<Target>(entity);

			if (!ecs.hasEntity(targetComp.entity)) {
				ecs.removeComponent<Target>(entity);
			}

			// TODO: Need to check weapon range and LOS. but probably somewhere else, since we then need to decide if we
			// can even shoot or if we need to move to a appropriate position.

			if (ew.magazineSize == 0) {
				ew.reloadTimeAccumulator += static_cast<float>(deltaTime);
				isReloading = true;
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

			Vec2f start = ecs.getComponent<Positionable>(entity).position + (TILE_SIZE / 2);
			Vec2f targetPos = ecs.getComponent<Positionable>(targetComp.entity).position + (TILE_SIZE / 2);

			auto rb = ecs.getComponent<RigidBody>(targetComp.entity);
			Vec2f targetVelocity = (rb.nextPosition - rb.startPosition).norm() * WALK_SPEED;
			Vec2f leadPos = calculateLead(start, targetPos, wdata.speed, targetVelocity);
			Vec2f projectileVelocity = (leadPos - start).norm() * wdata.speed;

			spawnProjectile(ecs, start, projectileVelocity, entity, ew.weaponId);
			isShooting = true; 
		}
	}
};
