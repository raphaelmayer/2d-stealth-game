#pragma once

#include "../engine/types/Vec2f.hpp"
#include "../items/WeaponMetadata.hpp"
#include <easys/easys.hpp>

// Projectile is kind of a confusing name, since this is just a special component for  projectile-type entities. A
// projectile still needs a position component, etc. But for now I don't have a better name, so it's just that.
// We could name it something like KinematicBody.
struct Projectile {
	Vec2f startPosition;
	Vec2f velocity; // might be better than having target position and needing to recalculate velocity.

	float range;
	int damage;

	Easys::Entity shooter;  // entity id of the entity that shot the weapon
	WeaponID weapon; // weapon id of the weapon that was shot
};