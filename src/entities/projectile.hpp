#pragma once

#include "../components/Collider.hpp"
#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../components/Renderable.hpp"
#include "../constants.hpp"
#include "../engine/types.hpp"
#include "../items/WeaponDatabase.hpp"
#include "../modules/Utils.hpp"
#include <easys/easys.hpp>

void spawnProjectile(Easys::ECS &ecs, Vec2f start, Vec2f velocity, Easys::Entity shooter, WeaponID weaponId)
{
	WeaponMetadata wd = WeaponDatabase::getInstance().get(weaponId);

	Easys::Entity entity = ecs.addEntity();
	ecs.addComponent<Projectile>(entity, {start, velocity, wd.range, wd.damage, shooter, weaponId});
	ecs.addComponent<Positionable>(entity, {start});
	ecs.addComponent<Renderable>(entity, {SPRITE_SHEET, Vec2i{0, 13}*TILE_SIZE, {4, 4}, {4, 4}});
	// TODO: giving projectiles colliders might lead to issues with physicssystem picking up and handling projectiles.
	// need to look into this more closely.
	// ecs.addComponent<Collider>(entity, {{3, 3}});
}