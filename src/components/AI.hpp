#pragma once

#include "../ai/AIRole.hpp"
#include "../ai/AIState.hpp"
#include "../ai/Blackboard.hpp"
#include "../behaviortree/BehaviorTree.hpp"
#include "../engine/Vec2d.hpp"
#include <vector>

struct AI {
	// AIRole role = AIRole::Neutral;
	AIState state = AIState::Unaware;
	AIState previousState = AIState::Unaware;
	double detectionTime = 0;								// for state transitions
	double searchTime = 0;									// for state transitions

	std::vector<Vec2d> path = {};							// Current path to the target
	Vec2d targetPosition;									// Current target
	
	Blackboard blackboard;
	// std::shared_ptr<BehaviorTree> behaviorTree; // Behavior tree instance

	AI(Blackboard blackboard_) : blackboard(blackboard_) {}

	// TODO: handle serialization, when the time comes
	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(targetPosition);
	}
};