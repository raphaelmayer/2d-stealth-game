#pragma once

#include "../components/Animatable.hpp"
#include "../components/Collider.hpp"
#include "../components/Controllable.hpp"
#include "../components/Health.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types/Vec2f.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/Utils.hpp"
#include <string>

Entity instantiateBaseEntity(ECSManager &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH,
                             int playerSpriteSheetY = 0)
{
	Entity base = ecs.addEntity();

	ecs.addComponent(base, Positionable{Utils::toFloat(positionInTiles) * TILE_SIZE});
	ecs.addComponent(base, Rotatable{rotation});
	ecs.addComponent(base, RigidBody{false, false, positionInTiles * TILE_SIZE, positionInTiles * TILE_SIZE});
	ecs.addComponent(base, Animatable{PLAYER_STANDING_ANIMATION_NUMBER,
	                                  {playerSpriteSheetY, playerSpriteSheetY + 64, playerSpriteSheetY + 2 * 64,
	                                   playerSpriteSheetY + 64}});
	ecs.addComponent(base, Renderable{HERO_SHEET, {2 * 32, playerSpriteSheetY}, {32, 64}, {16, 32}});
	ecs.addComponent(base, Collider{});
	ecs.addComponent(base, Health{100, 100});

	return base;
}
