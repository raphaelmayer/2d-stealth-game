#pragma once

#include "../ai/AIState.hpp"
#include "../behaviortree/ControlNodes.hpp"
#include "../components/AI.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/BTManager.hpp"
#include "../modules/MapManager.hpp"
#include "../systems/AIPerceptionSystem.hpp"
#include "../systems/System.hpp"
#include <cmath>
#include <iostream>

// The AISystem class is responsible for coordinating all AI-related subsystems, including perception, decision-making,
// and actions.
class AISystem final : public System {
  public:
	AISystem(BTManager &btManager_, const MapManager &mapManager) : btManager(btManager_), perceptionSystem(mapManager)
	{
	}

	void update(ECSManager &ecs, const double deltaTime)
	{
		const std::set<Entity> entities = ecs.getEntities();

		perceptionSystem.update(ecs, deltaTime);
		btManager.setGlobalTreeValue<double>("deltaTime", deltaTime);
		// Update state machine for high-level decisions (currently done within the loop down below)
		// stateMachine.updateState(ecs, entity, deltaTime);

		// act
		for (const Entity &entity : entities) {
			if (ecs.hasComponent<AI>(entity)) {
				AI &ai = ecs.getComponent<AI>(entity);

				if (ecs.hasComponent<Vision>(entity)) {
					Vision &vision = ecs.getComponent<Vision>(entity);

					updateHighLevelAIState(ecs, entity, deltaTime);

					btManager.tickTree(entity);
				}
			}
		}
	}

  private:
	// TODO: Create dedicated state machine at some point. Currently everything related to state is stored in the AI
	// component.
	void updateHighLevelAIState(ECSManager &ecs, Entity entity, double deltaTime)
	{
		// These components are a given at this point, because we check above. Possible error down the line.
		Vec2i &position = ecs.getComponent<Positionable>(entity).position;
		Vision &vision = ecs.getComponent<Vision>(entity);
		AI &ai = ecs.getComponent<AI>(entity);

		const AIState currentState = ai.state;
		switch (currentState) {
		case AIState::Unaware:
			if (!vision.visibleEnemies.empty()) {
				ai.previousState = currentState;
				ai.state = AIState::Detecting;
			}
			break;

		case AIState::Detecting:
			if (vision.visibleEnemies.empty()) {
				ai.searchTime += deltaTime;
				ai.detectionTime = 0;
				ai.state = ai.previousState;
				ai.previousState = currentState;
			} else {
				ai.detectionTime += deltaTime;
				if (ai.detectionTime >= detectionThreshold) {
					ai.detectionTime = 0;
					ai.originalPosition = (ai.previousState == AIState::Unaware)
					                          ? position
					                          : ai.originalPosition; // TODO: Should only happen on first detect, bc we
					                                                 // want to save the position of the idle state
					ai.previousState = currentState;
					ai.state = AIState::Engaging;
				}
			}
			break;

		case AIState::Searching:
			if (!vision.visibleEnemies.empty()) {
				ai.searchTime += deltaTime;
				ai.previousState = currentState;
				ai.state = AIState::Detecting;
			} else if (ai.searchTime >= searchTimeout) {
				ai.searchTime = 0;
				ai.previousState = currentState;
				ai.state = AIState::Unaware;
			} else {
				ai.searchTime += deltaTime;
			}
			break;

		case AIState::Engaging:
			ai.searchTime = 0;
			if (vision.visibleEnemies.empty()) {
				ai.previousState = currentState;
				ai.state = AIState::Searching;
			}
			break;

		case AIState::Fleeing:
			// Transition logic for fleeing (e.g., no threat detected)
			break;
		}
	}

	AIPerceptionSystem perceptionSystem;

	// AIStateMachine stateMachine;

	BTManager &btManager;

	// TODO: Temporary variables
	const double detectionThreshold = 2.0; // Time to transition to searching
	const double searchTimeout = 5.0;      // Timeout for searching
	const double engageTimeout = 5.0;      // Timeout for engaging (when no enemy is detected)
};