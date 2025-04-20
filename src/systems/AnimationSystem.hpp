#include "../components/AI.hpp"
#include "../components/Animatable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../engine/Engine.hpp"
#include "../map/MapManager.hpp"
#include "../modules/Camera.hpp"
#include "../modules/Utils.hpp"
#include "System.hpp"
#include <cmath>
#include <easys/easys.hpp>
#include <functional>
#include <iostream>

// The AnimationSystem is TODO.
class AnimationSystem final : public System {
  public:
	AnimationSystem(const Engine &engine, const MapManager &mapManager, const Camera &camera)
	    : engine_(engine), mapManager_(mapManager), camera_(camera)
	{
	}

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		const std::set<Easys::Entity> &entities = ecs.getEntities();
		for (const Easys::Entity &entity : entities) {
			if (ecs.hasComponent<Animatable>(entity) && ecs.hasComponent<Renderable>(entity)) {
				auto &sourcePosition = ecs.getComponent<Renderable>(entity).sourcePosition;
				auto &animatable = ecs.getComponent<Animatable>(entity);
				animatable.timeElapsed += deltaTime;
				handleAnimation(ecs, entity, animatable, sourcePosition.y);
			}
		}
	}

  private:
	void handleAnimation(Easys::ECS &ecs, const Easys::Entity entity, Animatable &animatable, int &spriteSrcY) const
	{
		if (animatable.timeElapsed > ANIMATION_UPDATE_RATE_IN_SECONDS) {
			animatable.timeElapsed = 0;
			if (ecs.hasComponent<RigidBody>(entity) && ecs.getComponent<RigidBody>(entity).isMoving) {
				animatable.currentAnimation += 1;
				if (animatable.currentAnimation >= PLAYER_NUMBER_ANIMATIONS) {
					animatable.currentAnimation = 0;
				}
			} else {
				animatable.currentAnimation = PLAYER_STANDING_ANIMATION_NUMBER;
			}
		}
		spriteSrcY = animatable.animationAdresses[animatable.currentAnimation];
	}

	const Engine &engine_;
	const MapManager &mapManager_; // Maybe needed in the future if we want to animate tiles (e.g. water, trees)
	const Camera &camera_;
};
