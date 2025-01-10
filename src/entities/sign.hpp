#pragma once

#include "../components/Collider.hpp"
#include "../components/Interactable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Vec2i.hpp"
#include <string>

Entity instantiateSignEntity(ECSManager &ecs, Vec2i positionInTiles, const std::string &text = "")
{
	Entity npc = ecs.addEntity();

	ecs.addComponent(npc, Positionable{{positionInTiles.x * TILE_SIZE, positionInTiles.y * TILE_SIZE}});
	ecs.addComponent(npc, Renderable{{5*TILE_SIZE,1*TILE_SIZE},{TILE_SIZE, TILE_SIZE}, 0});
	ecs.addComponent(npc, Collider{});
	ecs.addComponent(npc, Interactable{text});

	return npc;
}