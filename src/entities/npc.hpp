#pragma once

#include "../components/Animatable.hpp"
#include "../components/Collider.hpp"
#include "../components/Controllable.hpp"
#include "../components/Interactable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Vec2d.hpp"
#include <string>

// for playerSpriteSheetY use one of the constants PLAYER_{RED|BLUE|WHITE}_SPRITE_SHEET_Y
Entity instantiateNPCEntity(ECSManager &ecs, Vec2d positionInTiles, Rotation rotation = SOUTH,
                            int playerSpriteSheetY = PLAYER_SPRITE_SHEET_Y_RED, const std::string &text = "PLACEHOLDER TEXT")
{
	Entity npc = ecs.addEntity();

	ecs.addComponent(npc, Positionable{{positionInTiles.x * TILE_SIZE, positionInTiles.y * TILE_SIZE}});
	ecs.addComponent(npc, Rotatable{rotation});
	ecs.addComponent(npc, RigidBody{false, positionInTiles, positionInTiles});
	ecs.addComponent(npc, Animatable{PLAYER_STANDING_ANIMATION_NUMBER,
	                                 {playerSpriteSheetY, playerSpriteSheetY + PLAYER_SIZE_Y,
	                                  playerSpriteSheetY + 2 * PLAYER_SIZE_Y, playerSpriteSheetY + PLAYER_SIZE_Y}});
	ecs.addComponent(npc, Renderable{{2 * PLAYER_SIZE_X, playerSpriteSheetY}, {PLAYER_SIZE_X, PLAYER_SIZE_Y}, -8});
	ecs.addComponent(npc, Collider{});
	ecs.addComponent(npc, Interactable{text});

	return npc;
}