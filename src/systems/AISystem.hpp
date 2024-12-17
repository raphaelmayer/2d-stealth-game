#pragma once

#include "../ai/TurnToNode.hpp"
#include "../ai/AIState.hpp"
#include "../behaviortree/ControlNodes.hpp"
#include "../components/AI.hpp"
#include "../components/Vision.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Vec2d.hpp"
#include "../modules/MapManager.hpp"
#include "../systems/AIPerceptionSystem.hpp"
#include "../systems/System.hpp"
#include <cmath>
#include <iostream>
#include "../ai/IsEnemyVisibleNode.hpp"
#include "../ai/WaitForNode.hpp"

// The AISystem class is responsible for coordinating all AI-related subsystems, including perception, decision-making,
// and actions.
class AISystem final : public System {
  public:
	AISystem(const MapManager &mapManager) : perceptionSystem(mapManager) {  }

	void update(ECSManager &ecs, const double deltaTime)
	{
		const std::set<Entity> entities = ecs.getEntities();

		perceptionSystem.update(ecs, deltaTime);

		// Update state machine for high-level decisions
		// stateMachine.updateState(ecs, entity, deltaTime);
		
		// act
		for (const Entity &entity : entities) {
			if (ecs.hasComponent<AI>(entity)) {
				AI &ai = ecs.getComponent<AI>(entity);
				if (ecs.hasComponent<Vision>(entity)) {
					Vision &vision = ecs.getComponent<Vision>(entity);
					
					updateState(ai, vision, deltaTime);

					ai.blackboard.deltaTime = deltaTime;

					switch (ai.state) {
					case AIState::Unaware:
							ubt.tick(ai.blackboard);
							break;

						case AIState::Detecting:
							dbt.tick(ai.blackboard);
							break;

						case AIState::Searching:
							break;

						case AIState::Engaging:
							break;

						case AIState::Fleeing:
							break;
						}
				}
			}
		}
	}

  private:
	// TODO: Create dedicated state machine at some point. Currently everything related to state is stored in the AI component.
	void updateState(AI &ai, Vision &vision, double deltaTime)
	{
		const AIState currentState = ai.state;
		switch (currentState) {
		case AIState::Unaware:
			if (!vision.visibleEntities.empty()) {
				ai.previousState = currentState;
				ai.state = AIState::Detecting;
			}
			break;

		case AIState::Detecting:
			if (vision.visibleEntities.empty()) {
				ai.searchTime += deltaTime;
				ai.detectionTime = 0;
				ai.state = ai.previousState;
				ai.previousState = currentState;
			} else {
				ai.detectionTime += deltaTime;
				if (ai.detectionTime >= detectionThreshold) {
					ai.detectionTime = 0;
					ai.previousState = currentState;
					ai.state = AIState::Engaging;
				}
			}
			break;

		case AIState::Searching:
			if (!vision.visibleEntities.empty()) {
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
			if (vision.visibleEntities.empty()) {
				ai.previousState = currentState;
				ai.state = AIState::Searching;
			}
			break;

		case AIState::Fleeing:
			// Transition logic for fleeing (e.g., no threat detected)
			break;
		}
	}

	BehaviorTree createUBT()
	{
		auto mainTreeRoot = std::make_unique<SelectorNode>();

		auto sequence = std::make_unique<SequenceNode>();
		sequence->addChild(std::make_unique<TurnToNode>());
		sequence->addChild(std::make_unique<WaitForNode>());
		mainTreeRoot->addChild(std::move(sequence));

		BehaviorTree tree(std::move(mainTreeRoot));

		return tree;
	}

	BehaviorTree createDBT()
	{
		auto mainTreeRoot = std::make_unique<SelectorNode>();
		
		auto sequence = std::make_unique<SequenceNode>();
		sequence->addChild(std::make_unique<IsEnemyVisibleNode>());
		sequence->addChild(std::make_unique<TurnToNode>());
		// mainTreeRoot->addChild(std::move(subtree)); // Add the subtree
		mainTreeRoot->addChild(std::move(sequence));

		BehaviorTree tree(std::move(mainTreeRoot));

		return tree;
	}

	AIPerceptionSystem perceptionSystem;
	// AIStateMachine stateMachine;
	BehaviorTree ubt = createUBT();
	BehaviorTree dbt = createDBT();

	// TODO: Temporary variables
	const double detectionThreshold = 2.0; // Time to transition to searching
	const double searchTimeout = 5.0;      // Timeout for searching
	const double engageTimeout = 5.0;      // Timeout for engaging (when no enemy is detected)
};