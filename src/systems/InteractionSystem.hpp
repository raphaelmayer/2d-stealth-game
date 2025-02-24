#include "../components/Animatable.hpp"
#include "../components/Collectable.hpp"
#include "../components/Collider.hpp"
#include "../components/Controllable.hpp"
#include "../components/Interactable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../components/Tombstone.hpp"
#include "../constants.hpp"
#include "../ecs/Entity.hpp"
#include "../engine/Engine.hpp"
#include "../ui/MenuStack.hpp"
#include "../ui/TextDialog.hpp"
#include "System.hpp"
#include <iostream>

class InteractionSystem final : public System {
  public:
	InteractionSystem(Engine &engine, MenuStack &menuStack) : engine_(engine), menuStack_(menuStack) {}

	void update(ECSManager &ecs, const double deltaTime) override
	{
		const std::set<Entity> &entities = {PLAYER}; // no need to iterate over all entities

		for (const Entity &entity : entities) {
			if (ecs.hasComponent<Controllable>(entity)) {
				auto &controllable = ecs.getComponent<Controllable>(entity);
				auto &otherEntity = controllable.isInInteractionWith;

				// if player is not yet in an interaction and tries to interact
				if (otherEntity == 0 && controllable.isTryingToStartInteraction) {
					handleStartInteraction(ecs, entity);
					controllable.isTryingToStartInteraction = false;

					// if player is in an interaction and tries to quit it
				} else if (otherEntity != 0 && controllable.isTryingToEndInteraction) {
					handleEndInteraction(ecs, entity);
					controllable.isTryingToEndInteraction = false;
				}
			}
		}
	}

  private:
	void handleStartInteraction(ECSManager &ecs, const Entity entity)
	{
		if (ecs.hasComponent<Rotatable>(entity) && ecs.hasComponent<Positionable>(entity)) {
			auto &rotatable = ecs.getComponent<Rotatable>(entity);
			auto &positionable = ecs.getComponent<Positionable>(entity);

			// look for target entity (entity on the tile, which the player is facing)
			Vec2f targetPosition = calculateTargetPosition(positionable.position, rotatable.rotation);
			Entity targetEntity = findTargetEntity(ecs, targetPosition);

			// if there is an entity, it is interactable and has no tombstone component
			if (targetEntity != 0 && ecs.hasComponent<Interactable>(targetEntity)
			    && !ecs.hasComponent<Tombstone>(targetEntity)) {

				// turn target entity to player, if it is rotatable
				if (ecs.hasComponent<Rotatable>(targetEntity))
					handleTargetRotation(ecs, entity, targetEntity);

				Interactable &interactable = ecs.getComponent<Interactable>(targetEntity);
				TextDialog db = TextDialog(engine_, menuStack_, interactable.text);
				menuStack_.push(std::make_unique<TextDialog>(db));

				// Item gets collected
				if (ecs.hasComponent<Collectable>(targetEntity)) {
					ecs.getComponent<Inventory>(entity).items.push_back(targetEntity);


					// add armor to players stats
					ecs.getComponent<Stats>(entity).armor += ecs.getComponent<Collectable>(targetEntity).armor;

					// Remove components from collectable entity
					ecs.removeComponent<Positionable>(targetEntity);
					ecs.removeComponent<RigidBody>(targetEntity);
				}

				ecs.getComponent<Controllable>(entity).isInInteractionWith = targetEntity;
			}
		}
	}

	void handleEndInteraction(ECSManager &ecs, const Entity entity)
	{
		Entity &targetEntity = ecs.getComponent<Controllable>(entity).isInInteractionWith;

		// turn targeted entity back to its default rotation
		if (ecs.hasComponent<Rotatable>(lastTargetEntity_))
			ecs.getComponent<Rotatable>(lastTargetEntity_).rotation = lastTargetEntityRotation;

		// reset last target entity
		lastTargetEntity_ = -1;

		ecs.getComponent<Interactable>(targetEntity).isTextVisible = false;
		targetEntity = 0;
	}

	static Vec2f calculateTargetPosition(const Vec2f position, const int rotation)
	{
		switch (rotation) {
		case NORTH:
			return Vec2f{position.x, position.y - TILE_SIZE};
		case EAST:
			return Vec2f{position.x + TILE_SIZE, position.y};
		case SOUTH:
			return Vec2f{position.x, position.y + TILE_SIZE};
		case WEST:
			return Vec2f{position.x - TILE_SIZE, position.y};
		default:
			return position;
		}
	}

	static Entity findTargetEntity(ECSManager &ecs, const Vec2f targetPosition)
	{
		const std::set<Entity> &entities = ecs.getEntities();

		for (const Entity &tempEntity : entities) {
			if (ecs.hasComponent<Positionable>(tempEntity)) {
				auto &positionable = ecs.getComponent<Positionable>(tempEntity);
				if (positionable.position == targetPosition) {
					return tempEntity;
				}
			}
		}
		return -1;
	}

	void handleTargetRotation(ECSManager &ecs, const Entity entity, Entity targetEntity)
	{
		// save target entity and its rotation, to turn it back later
		lastTargetEntity_ = targetEntity;
		lastTargetEntityRotation = ecs.getComponent<Rotatable>(targetEntity).rotation;
		Rotation targetRotation;
		switch (ecs.getComponent<Rotatable>(entity).rotation) {
		case NORTH:
			targetRotation = SOUTH;
			break;
		case EAST:
			targetRotation = WEST;
			break;
		case SOUTH:
			targetRotation = NORTH;
			break;
		case WEST:
			targetRotation = EAST;
			break;
		}
		ecs.getComponent<Rotatable>(targetEntity).rotation = targetRotation;
	}

	Engine &engine_;
	MenuStack &menuStack_;
	Entity lastTargetEntity_ = -1;
	Rotation lastTargetEntityRotation = NORTH;
};
