#pragma once

#include "../engine/types/Vec2i.hpp"

// This component is used for movement. An end position is set by some other system (e.g. InputSystem)
// and the PhysicsSystem will then move the entity accordingly.
// Note:
// If the entity is not moving, end position equals transform.position.
// If the entity is moving, end position equals the immediate next tile.
struct RigidBody {
	bool isMoving = false;
	bool isShooting = false; // rigidBody is missleading so this should not be here but stays here for now as we need to change to statemachine anyway
	Vec2i startPosition; // in pixel space. unused as of now, but might be handy in the future
	Vec2i endPosition;   // in pixel space
	// TODO: Remove progress and accumulator as they are no longer used.
	float progress = 0;
	float accumulator = 0; // accumulates movement increments smaller than 1 pixel

	/*template <class Archive>
	void serialize(Archive &archive)
	{
		archive(isMoving, isShooting, startPosition, endPosition, progress, accumulator);
	}*/
};

// Due to the introduction of deltaTime movement increments can become a lot smaller than 1.
// This is an issue, because we use int for all position-related calculations.
// To work around this we do sub-pixel accumulation of these movementAmounts in the RigidBody component.
// Making Vec2d generic or changing to float produces a lot of type mismatches and errors, so this is the solution.
// It is good enough I suppose.
