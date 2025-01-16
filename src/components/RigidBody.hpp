#pragma once

#include "../engine/types/Vec2i.hpp"

// This component is used for movement. An end position is set by some other system (e.g. InputSystem)
// and the PhysicsSystem will then move the entity accordingly.
// Note:
// If the entity is not moving, end position equals transform.position.
// If the entity is moving, end position equals the immediate next tile.
struct RigidBody {
	bool isMoving = false;
	Vec2f startPosition;
	Vec2f endPosition;
	float progress = 0;
	float accumulator = 0; // accumulates movement increments smaller than 1 pixel

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(isMoving, startPosition, endPosition, progress, accumulator);
	}
};

// Due to the introduction of deltaTime movement increments can become a lot smaller than 1.
// This is an issue, because we use int for all position-related calculations.
// To work around this we do sub-pixel accumulation of these movementAmounts in the RigidBody component.
// Making Vec2d generic or changing to float produces a lot of type mismatches and errors, so this is the solution.
// It is good enough I suppose.
