#include "../components/Controllable.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../modules/Camera.hpp"
#include "System.hpp"
#include <SDL.h>
#include <set>

class InputSystem final : public System {
  public:
	explicit InputSystem(const Engine &engine, Camera &camera) : engine_(engine), camera_(camera) {}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = {PLAYER}; // no need to iterate over all entities
		std::array<KeyState, SDL_NUM_SCANCODES> keyState = engine_.getKeyStates();
		Vec2i mouseWheelDelta = engine_.getMouseWheelDelta();
		Vec2i mousePos = engine_.getMousePosition();

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Controllable>(entity) && ecs.hasComponent<RigidBody>(entity)) {
				auto &controllable = ecs.getComponent<Controllable>(entity);
				auto &rigidBody = ecs.getComponent<RigidBody>(entity);

				handleCamera(keyState, mouseWheelDelta, mousePos);

				// only allow input if player is not currently moving
				if (!rigidBody.isMoving) {
					// if (rigidBody.endPosition == position) {
					//  allow movement or starting an interaction, if player is not in an interaction
					if (controllable.isInInteractionWith == 0) {
						if (keyState[SDL_SCANCODE_RETURN].pressed) {
							controllable.isTryingToStartInteraction = true;
						} else {
							handleMovement(ecs, entity, keyState);
						}
						// allow ending an interaction, if player is in an interaction
					} else {
						if (keyState[SDL_SCANCODE_RETURN].pressed) {
							controllable.isTryingToEndInteraction = true;
						}
					}
				}
			}
		}
	}

  private:
	const Engine &engine_;
	Camera &camera_;

	static void handleMovement(ECSManager &ecs, Entity entity, const std::array<KeyState, SDL_NUM_SCANCODES> &keyState)
	{
		if (ecs.hasComponent<Positionable>(entity)) {
			auto &positionable = ecs.getComponent<Positionable>(entity);
			auto &rigidBody = ecs.getComponent<RigidBody>(entity); // existance check happens earlier
			Vec2i velocity{0, 0};

			if (keyState[SDL_GetScancodeFromKey(SDLK_w)].held) {
				velocity.y = -1;
			} else if (keyState[SDL_GetScancodeFromKey(SDLK_d)].held) {
				velocity.x = 1;
			} else if (keyState[SDL_GetScancodeFromKey(SDLK_s)].held) {
				velocity.y = 1;
			} else if (keyState[SDL_GetScancodeFromKey(SDLK_a)].held) {
				velocity.x = -1;
			}

			// Movement mechanism: if endPosition != position => move
			rigidBody.endPosition = Utils::toGrid(positionable.position) + velocity * TILE_SIZE;
		}
	}

	void handleCamera(const std::array<KeyState, SDL_NUM_SCANCODES> &keyState, Vec2i mouseWheelDelta,
	                  Vec2i mousePosition)
	{
		if (mouseWheelDelta.y > 0)
			camera_.zoomIn();
		if (mouseWheelDelta.y < 0)
			camera_.zoomOut();

		if (mousePosition.x < 10)
			camera_.move(CamDirection::LEFT);
		if (mousePosition.x > engine_.getWindowSize().x - 10)
			camera_.move(CamDirection::RIGHT);
		if (mousePosition.y < 10)
			camera_.move(CamDirection::UP);
		if (mousePosition.y > engine_.getWindowSize().y - 10)
			camera_.move(CamDirection::DOWN);
	}
};
