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
	explicit InputSystem(const Engine &engine, Camera &camera) : engine_(engine), camera_(camera) {}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = {PLAYER}; // no need to iterate over all entities
		std::array<KeyState, SDL_NUM_SCANCODES> keyStates = engine_.getKeyStates();
		std::array<KeyState, NUM_MOUSE_BUTTONS> mouseKeyStates = engine_.getMouseButtonStates();
		Vec2i mouseWheelDelta = engine_.getMouseWheelDelta();
		Vec2i mousePos = engine_.getMousePosition();

		handleCamera(mouseWheelDelta, mousePos, keyStates);
		// Utils::print(Utils::toTileSize(screenToWorld(Utils::toFloat(engine_.getMousePosition()))));

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Controllable>(entity) && ecs.hasComponent<RigidBody>(entity)) {
				auto &controllable = ecs.getComponent<Controllable>(entity);
				auto &rigidBody = ecs.getComponent<RigidBody>(entity);

				playerControllerMouse(ecs, entity, mouseKeyStates, deltaTime);
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

	void playerControllerMouse(ECSManager &ecs, Entity entity, const std::array<KeyState, NUM_MOUSE_BUTTONS> &keyStates,
	                           const double deltaTime)
	{
		handleSelection(ecs, keyStates);

		if (keyStates[RIGHT_MOUSE_BUTTON].pressed) {
			Vec2f mousePos = camera_.screenToWorld(Utils::toFloat(engine_.getMousePosition()));
			Rectf r = {mousePos.x, mousePos.y, 1, 1};
			std::vector<Entity> entities = getCollidingEntities(ecs, r);

			if (!entities.empty()) {
				// if (enemy)
				Target targetComponent{entities[0]};
				ecs.addComponent<Target>(entity, targetComponent);
				ecs.addComponent<Pathfinding>(entity, {}); // cancel any movement
			}

			else {
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
			const Vec2f pos = ecs.getComponent<Positionable>(entity).position;
			Rectf entityRectangle{pos.x, pos.y, TILE_SIZE, TILE_SIZE}; // TODO: read size from component

			if (AABB::checkCollision(rect, entityRectangle)) {
				entities.push_back(entity);
			}
		}

		return entities;
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
		if (mousePosition.x > engine_.getScreenSize().x - 10)
			camera_.move(CamDirection::RIGHT);
		if (mousePosition.y < 10)
			camera_.move(CamDirection::UP);
		if (mousePosition.y > engine_.getScreenSize().y - 10)
			camera_.move(CamDirection::DOWN);
	}

	void handleEngagement(ECSManager &ecs, const Entity &entity, const Entity &other)
	{
		Target targetComponent{other};
		ecs.addComponent<Target>(entity, targetComponent);
		ecs.addComponent<Pathfinding>(entity, {}); // cancel any movement
	}
};
