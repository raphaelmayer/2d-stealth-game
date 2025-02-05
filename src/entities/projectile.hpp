#pragma once

#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../components/Renderable.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types.hpp"
#include "../modules/Utils.hpp"

void spawnProjectile(ECSManager &ecs, Vec2f start, Vec2f end, float velocity, Entity shooter)
{
	Entity entity = ecs.addEntity();
	ecs.addComponent<Projectile>(entity, {start, end, velocity, shooter});
	ecs.addComponent<Positionable>(entity, {start});
	ecs.addComponent<Renderable>(entity, {Vec2i{8, 1} * TILE_SIZE, {TILE_SIZE, TILE_SIZE}, 0}); // src, size, offset_y
}