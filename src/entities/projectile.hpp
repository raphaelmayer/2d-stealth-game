#pragma once

#include "../components/Positionable.hpp"
#include "../components/Projectile.hpp"
#include "../components/Renderable.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types.hpp"
#include "../modules/Utils.hpp"

void spawnProjectile(ECSManager &ecs, Vec2i start, Vec2i end, float velocity)
{
	Entity entity = ecs.addEntity();
	ecs.addComponent<Projectile>(entity, {start, end, velocity});
	ecs.addComponent<Positionable>(entity, {Utils::toFloat(start)});
	ecs.addComponent<Renderable>(entity, {Vec2i{8, 1} * TILE_SIZE, {TILE_SIZE, TILE_SIZE}, 0}); // src, size, offset_y
}