#include "../components/Controllable.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Target.hpp"
#include "../ecs/ECSManager.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../entities/projectile.hpp"
#include "../modules/AABB.hpp"
#include "../modules/Camera.hpp"
#include "../modules/Utils.hpp"
#include "System.hpp"
#include <SDL.h>
#include <set>

// Maybe InputSystem should not be a thing, since it is not like the other systems. It only handles inputs and updates
// the player entities accordingly. We'll see how the selection and multiple controllable entities pans out and then
// decide.
class InputSystem final : public System {
  public:
	explicit InputSystem(const Engine &engine, Camera &camera) : engine_(engine), camera_(camera)
	{
	}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = {PLAYER}; // no need to iterate over all entities
		const std::array<KeyState, SDL_NUM_SCANCODES> &keyStates = engine_.getKeyStates();
		const std::array<KeyState, NUM_MOUSE_BUTTONS> &mouseKeyStates = engine_.getMouseButtonStates();
		const Vec2i &mouseWheelDelta = engine_.getMouseWheelDelta();
		const Vec2i &mousePos = engine_.getMousePosition();

		handleCamera(mouseWheelDelta, mousePos, keyStates);

		handleSelection(ecs, mouseKeyStates);

		for (const Entity &entity : selection) {
			if (ecs.hasComponent<Controllable>(entity)) { // only allow control of entities with controllable component
				handleEntityControl(ecs, entity, mouseKeyStates, deltaTime);
			}
		}
	}

	std::vector<Entity> selection{}; // this vector holds the player selected entities.
	Vec2f start;
	Vec2f end;

  private:
	const Engine &engine_;
	Camera &camera_;

	static constexpr int LEFT_MOUSE_BUTTON = 0;
	static constexpr int RIGHT_MOUSE_BUTTON = 2;

	void handleEntityControl(ECSManager &ecs, Entity entity, const std::array<KeyState, NUM_MOUSE_BUTTONS> &keyStates,
	                         const double deltaTime) const
	{
		if (keyStates[RIGHT_MOUSE_BUTTON].pressed) {
			const Vec2f mousePos = camera_.screenToWorld(Utils::toFloat(engine_.getMousePosition()));
			const Rectf r = {mousePos.x, mousePos.y, 1, 1};
			const std::vector<Entity> entities = getCollidingEntities(ecs, r);

			if (!entities.empty()) {
				// tile is occupied by entity -> handle engagement
				// if (enemy)
				const Target targetComponent{entities[0]};
				ecs.addComponent<Target>(entity, targetComponent);
				ecs.addComponent<Pathfinding>(entity, {}); // cancel current path

			}

			else {
				// tile is free -> handle movement
				ecs.removeComponent<Target>(entity);
				auto &pathfinding = ecs.getComponent<Pathfinding>(entity);
				const Vec2f mousePos = Utils::toFloat(engine_.getMousePosition());
				pathfinding.targetPosition = Utils::toGrid(camera_.screenToWorld(mousePos));
			}
		}
	}

	void handleSelection(ECSManager &ecs, const std::array<KeyState, NUM_MOUSE_BUTTONS> &keyStates)
	{
		if (keyStates[LEFT_MOUSE_BUTTON].pressed) {
			selection.clear();
			Vec2f mousePos = Utils::toFloat(engine_.getMousePosition());
			start = camera_.screenToWorld(mousePos);
		}

		if (keyStates[LEFT_MOUSE_BUTTON].held) {
			Vec2f mousePos = Utils::toFloat(engine_.getMousePosition());
			end = camera_.screenToWorld(mousePos);
		}

		if (keyStates[LEFT_MOUSE_BUTTON].released) {
			Rectf r = Utils::vectorsToRectangle(start, end);
			std::vector<Entity> entities = getCollidingEntities(ecs, r);
			if (!entities.empty()) {
				for (const Entity &e : entities) {
					selection.push_back(e);
				}
			}

			start = {0.0f, 0.0f}, end = {0.0f, 0.0f};
		}
	}

	std::vector<Entity> getCollidingEntities(ECSManager &ecs, const Rectf &rect) const
	{
		std::vector<Entity> entities{};
		for (const auto &entity : ecs.getEntities()) {
			if (ecs.hasComponent<Collider>(entity) && ecs.hasComponent<Positionable>(entity)) {
			
				const Vec2f pos = ecs.getComponent<Positionable>(entity).position;
				const Vec2f size = Utils::toFloat(ecs.getComponent<Collider>(entity).size);
				// manipulating pos.y here because currently it is the top left of the bottom tile of any entity,
				// basically where the feet are.
				Rectf entityRectangle{pos.x, pos.y - (size.y - TILE_SIZE), size.x, size.y}; 

				if (AABB::checkCollision(rect, entityRectangle)) {
					entities.push_back(entity);
				}
			}
		}

		return entities;
	}

	void handleCamera(const Vec2i &mouseWheelDelta, const Vec2i &mousePosition,
	                  const std::array<KeyState, SDL_NUM_SCANCODES> &keyStates) const
	{
		if (mouseWheelDelta.y > 0)
			camera_.zoomIn();
		if (mouseWheelDelta.y < 0)
			camera_.zoomOut();

		if ((mousePosition.x < 10) || keyStates[SDL_SCANCODE_LEFT].held)
			camera_.move(CamDirection::LEFT);
		if ((mousePosition.x > engine_.getScreenSize().x - 10) || keyStates[SDL_SCANCODE_RIGHT].held)
			camera_.move(CamDirection::RIGHT);
		if ((mousePosition.y < 10) || keyStates[SDL_SCANCODE_UP].held)
			camera_.move(CamDirection::UP);
		if ((mousePosition.y > engine_.getScreenSize().y - 10) || keyStates[SDL_SCANCODE_DOWN].held)
			camera_.move(CamDirection::DOWN);
	}
};
