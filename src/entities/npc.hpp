#pragma once

#include "../components/AI.hpp"
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

Entity instantiateNPCEntity(ECSManager &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH,
                            const std::string &text = "PLACEHOLDER TEXT")
{
	Entity npc = instantiateBaseEntity(ecs, positionInTiles, rotation);
	ecs.addComponent<Vision>(npc, Vision{});
	ecs.addComponent<AI>(npc, AI{positionInTiles * TILE_SIZE});
	ecs.addComponent<Pathfinding>(npc, Pathfinding{});
	ecs.addComponent(npc, Interactable{text});

	return npc;
}