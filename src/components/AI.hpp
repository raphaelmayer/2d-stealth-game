#pragma once

#include "../ai/AIRole.hpp"
#include "../ai/AIState.hpp"
#include "../engine/types/Vec2i.hpp"
#include "behaviortree_cpp/bt_factory.h"
#include <memory>
#include <vector>

struct AI {
	Vec2i originalPosition; // The fallback position for AI to return to

	// AIRole role = AIRole::Neutral;
	AIState state = AIState::Unaware;
	AIState previousState = AIState::Unaware;

	float detectionTime = 0.0f;           // for state transitions
	float searchTime = 0.0f;              // for state transitions
	const float detectionThreshold = 2.0; // Time to transition to searching
	const float searchTimeout = 5.0;      // Timeout for searching

	// Vec2i targetPosition{-1, -1}; // Current target
	// std::vector<Vec2i> path = {}; // Current path to the target
	// int pathIndex = 0;

	// TODO: handle serialization, when the time comes
	template <class Archive>
	void serialize(Archive &archive)
	{
		archive();
	}
};