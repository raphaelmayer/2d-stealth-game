#pragma once

#include "../ai/AIRole.hpp"
#include "../ai/AIState.hpp"
#include "../ai/Blackboard.hpp"
#include "../behaviortree/BehaviorTree.hpp"
#include "../engine/types/Vec2i.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include <memory>
#include <vector>

struct AI {
	Vec2i originalPosition; // The fallback position for AI to return to

	// AIRole role = AIRole::Neutral;
	AIState state = AIState::Unaware;
	AIState previousState = AIState::Unaware;

	double detectionTime = 0; // for state transitions
	double searchTime = 0;    // for state transitions

	Vec2i targetPosition{-1, -1}; // Current target
	std::vector<Vec2i> path = {}; // Current path to the target
	int pathIndex = 0;

	// TODO: handle serialization, when the time comes
	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(targetPosition);
	}
};