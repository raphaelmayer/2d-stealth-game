#pragma once

#include "../engine/types/Rectf.hpp"

namespace AABB {
bool checkCollision(const Rectf &a, const Rectf &b)
{
	// If one rectangle is to the left of the other
	if (a.x + a.w < b.x || b.x + b.w < a.x)
		return false;

	// If one rectangle is above the other
	if (a.y + a.h < b.y || b.y + b.h < a.y)
		return false;

	// Otherwise, there is a collision
	return true;
}
} // namespace AABB