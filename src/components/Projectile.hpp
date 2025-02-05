#pragma once

#include "../engine/types/Vec2f.hpp"

// Projectile is kind of a confusing name, since this is just a special component for  projectile-type entities. A
// projectile still needs a position component, etc. But for now I don't have a better name, so it's just that. 
// We could name it something like KinematicBody.
struct Projectile {
	Vec2f startPosition;
	Vec2f targetPosition;
	float velocity;

	// TODO: We probably want a field to save the shooter
	// Entity shooter;
};