#pragma once

#include "../components/AI.hpp"
#include "../components/EquippedWeapon.hpp"
#include "../components/Interactable.hpp"
#include "../components/Pathfinding.hpp"
#include "../components/Vision.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types/Vec2f.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/Utils.hpp"
#include "entity.hpp"
#include <string>

// for playerSpriteSheetY use one of the constants PLAYER_{RED|BLUE|WHITE}_SPRITE_SHEET_Y
Entity instantiateNPCEntity(ECSManager &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH,
                            int playerSpriteSheetY = 0, const std::string &text = "PLACEHOLDER TEXT")
{
	Entity npc = instantiateBaseEntity(ecs, positionInTiles, rotation, playerSpriteSheetY);
	ecs.addComponent<Vision>(npc, Vision{});
	ecs.addComponent<AI>(npc, AI{positionInTiles * TILE_SIZE});
	ecs.addComponent<Pathfinding>(npc, Pathfinding{});
	ecs.addComponent(npc, Interactable{text});
	ecs.addComponent(npc, EquippedWeapon{1, 30}); // assault rifle with full mag

	return npc;
}