#pragma once

#include "../engine/types/Vec2i.hpp"

// Projectile is kind of a confusing name, since this is just a special component for  projectile-type entities. A
// projectile still needs a position component, etc. But for now I don't have a better name, so it's just that.
struct Projectile {
	Vec2i startPosition;
	Vec2i targetPosition;
	float velocity;
};