#include "../components/Controllable.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../entities/projectile.hpp"
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
		std::array<KeyState, SDL_NUM_SCANCODES> keyStates = engine_.getKeyStates();
		std::array<KeyState, NUM_MOUSE_BUTTONS> mouseKeyStates = engine_.getMouseButtonStates();
		Vec2i mouseWheelDelta = engine_.getMouseWheelDelta();
		Vec2i mousePos = engine_.getMousePosition();

		handleCamera(mouseWheelDelta, mousePos, keyStates);
		Utils::print(Utils::toTileSize(screenToWorld(Utils::toFloat(engine_.getMousePosition()))));

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Controllable>(entity) && ecs.hasComponent<RigidBody>(entity)) {
				auto &controllable = ecs.getComponent<Controllable>(entity);
				auto &rigidBody = ecs.getComponent<RigidBody>(entity);

				handleShooting(ecs, entity, mouseKeyStates);

				// only allow input if player is not currently moving
				if (!rigidBody.isMoving) {
					//  allow movement or starting an interaction, if player is not in an interaction
					if (controllable.isInInteractionWith == 0) {
						if (keyStates[SDL_SCANCODE_RETURN].pressed) {
							controllable.isTryingToStartInteraction = true;
						} else {
							handleMovement(ecs, entity, keyStates);
						}
					} else {
						// allow ending an interaction, if player is in an interaction
						if (keyStates[SDL_SCANCODE_RETURN].pressed) {
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

	void handleMovement_MOUSE(ECSManager &ecs, Entity entity, const std::array<KeyState, SDL_NUM_SCANCODES> &keyStates)
	{
		// When we switch to the RTS-style controller we need to move some things. Currently pathfinding related stuff
		// is in the AI component, since only AI uses pathfinding as of now.
		//
		// constexpr int RIGHT_MOUSE_BUTTON = 2;
		// if (keyStates[RIGHT_MOUSE_BUTTON].held == true) {
		//	auto &pathfinding = ecs.getComponent<Pathfinding>(entity);
		//	const Vec2f mousePos = Utils::toFloat(engine_.getMousePosition());
		//	pathfinding.targetPosition = screenToWorld(mousePos);
		// }
	}

	void handleMovement(ECSManager &ecs, Entity entity, const std::array<KeyState, SDL_NUM_SCANCODES> &keyStates)
	{
		if (ecs.hasComponent<Positionable>(entity)) {
			auto &positionable = ecs.getComponent<Positionable>(entity);
			auto &rigidBody = ecs.getComponent<RigidBody>(entity); // existence check happens earlier
			Vec2i velocity{0, 0};

			if (keyStates[SDL_GetScancodeFromKey(SDLK_w)].held) {
				velocity.y = -1;
			} else if (keyStates[SDL_GetScancodeFromKey(SDLK_d)].held) {
				velocity.x = 1;
			} else if (keyStates[SDL_GetScancodeFromKey(SDLK_s)].held) {
				velocity.y = 1;
			} else if (keyStates[SDL_GetScancodeFromKey(SDLK_a)].held) {
				velocity.x = -1;
			}

			// Movement mechanism: if endPosition != position => move
			rigidBody.endPosition = Utils::toGrid(positionable.position) + velocity * TILE_SIZE;
		}
	}

	void handleCamera(const Vec2i &mouseWheelDelta, const Vec2i &mousePosition,
	                  const std::array<KeyState, SDL_NUM_SCANCODES> &keyStates)
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

	void handleShooting(ECSManager &ecs, const Entity &entity, const std::array<KeyState, NUM_MOUSE_BUTTONS> &keyStates)
	{
		// This shooting mechanism should not be in InputSystem. What if we want to handle different weapons or
		// different actions depending on if an entity is an enemy or friendly.
		constexpr int RIGHT_MOUSE_BUTTON = 2;
		if (keyStates[RIGHT_MOUSE_BUTTON].held == true) {
			Vec2f start = ecs.getComponent<Positionable>(entity).position;
			Vec2f mouseScreenPos = Utils::toFloat(engine_.getMousePosition());
			Vec2f mouseWorldPos = screenToWorld(mouseScreenPos);
			float velocity = 300.f; // should be read from gun or something
			//WeaponID weaponId = ecs.getComponent<Loadout>(entity).mainhand;

			spawnProjectile(ecs, start, mouseWorldPos, velocity, entity);
			//spawnProjectile(ecs, start, mouseWorldPos, entity, weaponId);
		}
	}

	Vec2f screenToWorld(const Vec2f &vec) const
	{
		// Our renderscale is uniform across axis, so we just pick a dimension.
		return (vec / (camera_.getZoom() * engine_.getRenderScale().x)) + camera_.getPosition();
	}
};
