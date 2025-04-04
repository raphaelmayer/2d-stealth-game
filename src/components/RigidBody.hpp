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
	Vec2i nextPosition;   // in pixel space

	/*template <class Archive>
	void serialize(Archive &archive)
	{
		archive(isMoving, isShooting, startPosition, endPosition, progress, accumulator);
	}*/
};
