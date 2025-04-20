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
#include "../engine/types/Vec2f.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/Utils.hpp"
#include <easys/easys.hpp>
#include <string>

Easys::Entity instantiateBaseCharacter(Easys::ECS &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH,
                                int playerSpriteSheetY = 0)
{
	Easys::Entity base = ecs.addEntity();

	ecs.addComponent(base, Positionable{Utils::toFloat(positionInTiles) * TILE_SIZE});
	ecs.addComponent(
	    base,
	    Collider{TILE_SIZE,
	             TILE_SIZE * BASE_ENTITY_HEIGHT}); // adding size here as currently all entities have the same size and
	                                               // most entities will probably always be humans of size 2 anyway
	ecs.addComponent(base, Rotatable{rotation});
	ecs.addComponent(base, RigidBody{false, false, positionInTiles * TILE_SIZE, positionInTiles * TILE_SIZE});
	ecs.addComponent(base, Animatable{PLAYER_STANDING_ANIMATION_NUMBER,
	                                  {playerSpriteSheetY, playerSpriteSheetY + TILE_SIZE * BASE_ENTITY_HEIGHT,
	                                   playerSpriteSheetY + 2 * TILE_SIZE * BASE_ENTITY_HEIGHT,
	                                   playerSpriteSheetY + TILE_SIZE * BASE_ENTITY_HEIGHT}});
	ecs.addComponent(base, Renderable{HERO_SHEET,
	                                  {2 * TILE_SIZE, playerSpriteSheetY},
	                                  {TILE_SIZE, TILE_SIZE * BASE_ENTITY_HEIGHT},
	                                  {TILE_SIZE, TILE_SIZE * BASE_ENTITY_HEIGHT}});
	ecs.addComponent(base, Health{100, 100});

	return base;
}
