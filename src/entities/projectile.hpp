#pragma once

#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../components/Renderable.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types.hpp"
#include "../modules/Utils.hpp"
#include "../items/WeaponDatabase.hpp"

void spawnProjectile(ECSManager &ecs, Vec2f start, Vec2f end, Entity shooter, WeaponID weaponId)
{
	WeaponMetadata wd = WeaponDatabase::getInstance().get(weaponId);

	Entity entity = ecs.addEntity();
	ecs.addComponent<Projectile>(entity, {start, end, wd.velocity, shooter, weaponId});
	ecs.addComponent<Positionable>(entity, {start});
	ecs.addComponent<Renderable>(entity, {Vec2i{8, 1} * TILE_SIZE, {TILE_SIZE, TILE_SIZE}, 0}); // src, size, offset_y
}