#pragma once

#include "../components/Animatable.hpp"
#include "../components/Collider.hpp"
#include "../components/Controllable.hpp"
#include "../components/EquippedWeapon.hpp"
#include "../components/Interactable.hpp"
#include "../components/Inventory.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types/Vec2i.hpp"

// for playerSpriteSheetY use one of the constants PLAYER_{RED|BLUE|WHITE}_SPRITE_SHEET_Y
Entity instantiatePlayerEntity(ECSManager &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH,
                               int playerSpriteSheetY = PLAYER_SPRITE_SHEET_Y_RED)
{
	Entity player = ecs.addEntity();

	ecs.addComponent(player,
	                 Positionable{{(float)(positionInTiles.x * TILE_SIZE), (float)(positionInTiles.y * TILE_SIZE)}});
	ecs.addComponent(player, Rotatable{rotation});
	ecs.addComponent(player, RigidBody{false, false, positionInTiles, positionInTiles, 0, 0});
	ecs.addComponent(player, Animatable{PLAYER_STANDING_ANIMATION_NUMBER,
	                                    {playerSpriteSheetY, playerSpriteSheetY + PLAYER_SIZE_Y,
	                                     playerSpriteSheetY + 2 * PLAYER_SIZE_Y, playerSpriteSheetY + PLAYER_SIZE_Y}});
	ecs.addComponent(player, Renderable{{2 * PLAYER_SIZE_X, playerSpriteSheetY}, {PLAYER_SIZE_X, PLAYER_SIZE_Y}, -8});
	ecs.addComponent(player, Collider{});
	ecs.addComponent(player, Controllable{});
	ecs.addComponent(player, Inventory{});
	ecs.addComponent(player, EquippedWeapon{2,5}); // assault rifle with full mag

	return player;
}