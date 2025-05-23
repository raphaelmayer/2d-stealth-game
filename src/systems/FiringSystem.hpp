#include "../components/EquippedWeapon.hpp"
#include "../components/Positionable.hpp"
#include "../components/Target.hpp"
#include "../engine/Engine.hpp"
#include "../entities/projectile.hpp"
#include "../modules/Camera.hpp"
#include "../modules/StateMachine.hpp"
#include "System.hpp"
#include <easys/easys.hpp>
#include <set>

class FiringSystem final : public System {
  public:
	explicit FiringSystem(const Engine &engine) : engine_(engine)
	{
	}

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		const Vec2i mousePos = engine_.getMousePosition();
		const std::set<Easys::Entity> &entities = ecs.getEntities(); // no need to iterate over all entities

		for (const Easys::Entity &entity : entities) {
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
	// std::unordered_map<Easys::Entity, StateMachine> stateMachines;

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

	void handleFiring(Easys::ECS &ecs, const Easys::Entity &entity, const double deltaTime) const
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

		const bool &isMoving = ecs.getComponent<RigidBody>(entity).isMoving;
		if (!ecs.hasComponent<Target>(entity) || isMoving) {
			ew.warmupAccumulator = 0.f;
		}

		bool &isShooting = ecs.getComponent<RigidBody>(entity).isShooting;
		if (ecs.hasComponent<Target>(entity) /* && !isMoving*/) {
			Target targetComp = ecs.getComponent<Target>(entity);

			if (!ecs.hasEntity(targetComp.entity)) {
				ecs.removeComponent<Target>(entity);
				return;
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

			// stop moving (should this be in inputsystem / shootat node?)
			ecs.addComponent<Pathfinding>(entity, {}); // clear any planned movement

			if (isMoving) {
				return;
			}

			// aim (warmup)
			if (ew.warmupAccumulator < wdata.warmup) {
				ew.warmupAccumulator += static_cast<float>(deltaTime);
				return;
			}

			// fire
			ew.firerateAccumulator += static_cast<float>(deltaTime);
			--ew.magazineSize;

			// size should be read from projectile. we could also offset the spawn position by size / 2 in
			// spawnProjectile. There we would easily know the size.
			constexpr float prjOffset = 3.f / 2.f;
			Vec2f start = ecs.getComponent<Positionable>(entity).position + (TILE_SIZE / 2) - prjOffset;
			Vec2f targetPos = ecs.getComponent<Positionable>(targetComp.entity).position + (TILE_SIZE / 2) - prjOffset;

			auto rb = ecs.getComponent<RigidBody>(targetComp.entity);
			Vec2f targetVelocity = (rb.nextPosition - rb.startPosition).norm() * WALK_SPEED;
			Vec2f leadPos = calculateLead(start, targetPos, wdata.speed, targetVelocity);
			Vec2f projectileVelocity = (leadPos - start).norm() * wdata.speed;

			spawnProjectile(ecs, start, projectileVelocity, entity, ew.weaponId);
			isShooting = true;
		}
	}
};
