#pragma once

#include "../components/AI.hpp"
#include "../components/EquippedWeapon.hpp"
#include "../components/Interactable.hpp"
#include "../components/Pathfinding.hpp"
#include "../components/Vision.hpp"
#include "../constants.hpp"
#include "../engine/types/Vec2f.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/Utils.hpp"
#include "character.hpp"
#include <easys/easys.hpp>
#include <string>

Easys::Entity instantiateNPCEntity(Easys::ECS &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH,
                            const std::string &text = "PLACEHOLDER TEXT")
{
	Easys::Entity npc = instantiateBaseCharacter(ecs, positionInTiles, rotation);
	ecs.addComponent<Vision>(npc, Vision{});
	ecs.addComponent<AI>(npc, AI{positionInTiles * TILE_SIZE});
	ecs.addComponent<Pathfinding>(npc, Pathfinding{});
	ecs.addComponent(npc, Interactable{text});
	ecs.addComponent(npc, EquippedWeapon{1, 30}); // assault rifle with full mag

	return npc;
}