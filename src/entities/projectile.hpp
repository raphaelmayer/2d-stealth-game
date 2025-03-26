#pragma once

#include "../components/Collider.hpp"
#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../components/Renderable.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types.hpp"
#include "../items/WeaponDatabase.hpp"
#include "../modules/Utils.hpp"

void spawnProjectile(ECSManager &ecs, Vec2f start, Vec2f end, Entity shooter, WeaponID weaponId)
{
	WeaponMetadata wd = WeaponDatabase::getInstance().get(weaponId);

	Entity entity = ecs.addEntity();
	ecs.addComponent<Projectile>(entity, {start, end, wd.velocity, wd.damage, shooter, weaponId});
	ecs.addComponent<Positionable>(entity, {start});
	ecs.addComponent<Renderable>(entity, {SPRITE_SHEET, Vec2i{8, 1} * TILE_SIZE, {3, 3}, {3, 3}});
	// TODO: giving projectiles colliders might lead to issues with physicssystem picking up and handling projectiles.
	// need to look into this more closely.
	// ecs.addComponent<Collider>(entity, {{3, 3}});
}