#include "../components/Controllable.hpp"
#include "../components/Positionable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Target.hpp"
#include "../engine/Engine.hpp"
#include "../entities/projectile.hpp"
#include "../modules/AABB.hpp"
#include "../modules/Camera.hpp"
#include "../modules/SelectionManager.hpp"
#include "../modules/Utils.hpp"
#include "System.hpp"
#include <SDL.h>
#include <easys/easys.hpp>
#include <set>

// Maybe InputSystem should not be a thing, since it is not like the other systems. It only handles inputs and updates
// the player entities accordingly. We'll see how the selection and multiple controllable entities pans out and then
// decide.
class InputSystem final : public System {
  public:
	explicit InputSystem(const Engine &engine, Camera &camera, SelectionManager &selectionManager)
	    : engine_(engine), camera_(camera), selectionManager_(selectionManager)
	{
	}

	void update(Easys::ECS &ecs, const double deltaTime) override
	{
		const std::set<Easys::Entity> &entities = {PLAYER}; // no need to iterate over all entities
		const std::array<KeyState, SDL_NUM_SCANCODES> &keyStates = engine_.getKeyStates();
		const std::array<KeyState, NUM_MOUSE_BUTTONS> &mouseKeyStates = engine_.getMouseButtonStates();
		const Vec2i &mouseWheelDelta = engine_.getMouseWheelDelta();
		const Vec2i &mousePos = engine_.getMousePosition();

		handleCamera(mouseWheelDelta, mousePos, keyStates);

		handleSelection(ecs, mouseKeyStates);

		for (const Easys::Entity &entity : selectionManager_.getSelection()) {
			if (ecs.hasComponent<Controllable>(entity)) { // only allow control of entities with controllable component
				handleEntityControl(ecs, entity, mouseKeyStates, deltaTime);
			}
		}
	}

  private:
	const Engine &engine_;
	Camera &camera_;
	SelectionManager &selectionManager_;

	static constexpr int LEFT_MOUSE_BUTTON = 0;
	static constexpr int RIGHT_MOUSE_BUTTON = 2;

	void handleEntityControl(Easys::ECS &ecs, Easys::Entity entity,
	                         const std::array<KeyState, NUM_MOUSE_BUTTONS> &keyStates, const double deltaTime) const
	{
		if (keyStates[RIGHT_MOUSE_BUTTON].pressed) {
			const Vec2f mousePos = camera_.screenToWorld(Utils::toFloat(engine_.getMousePosition()));
			const Rectf r = {mousePos.x, mousePos.y, 1, 1};
			const std::vector<Easys::Entity> entities = getCollidingEntities(ecs, r);

			// we need to decide how we want to handle engagements for player characters. a small behaviour tree could
			// be a good choice especially since we can reuse AI nodes.

			if (!entities.empty()) {
				// tile is occupied by entity -> handle engagement
				if (entity != entities[0]) { // don't allow suicide
					const Target targetComponent{entities[0]};
					ecs.addComponent<Target>(entity, targetComponent);

					// cancel current path. currently done in firingsystem
					// ecs.addComponent<Pathfinding>(entity, {});
				}

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

	void handleSelection(Easys::ECS &ecs, const std::array<KeyState, NUM_MOUSE_BUTTONS> &keyStates)
	{
		if (keyStates[LEFT_MOUSE_BUTTON].pressed) {
			const Vec2f mousePos = Utils::toFloat(engine_.getMousePosition());
			selectionManager_.setStart(camera_.screenToWorld(mousePos));
		}

		if (keyStates[LEFT_MOUSE_BUTTON].held) {
			const Vec2f mousePos = Utils::toFloat(engine_.getMousePosition());
			selectionManager_.setEnd(camera_.screenToWorld(mousePos));
		}

		if (keyStates[LEFT_MOUSE_BUTTON].released) {
			const Rectf selectionRect = Utils::vectorsToRectangle(selectionManager_.getStart(), selectionManager_.getEnd());
			selectionManager_.setSelection(getCollidingEntities(ecs, selectionRect));

			selectionManager_.setStart({0.f, 0.f});
			selectionManager_.setEnd({0.f, 0.f});
		}
	}

	std::vector<Easys::Entity> getCollidingEntities(Easys::ECS &ecs, const Rectf &rect) const
	{
		std::vector<Easys::Entity> entities{};
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
