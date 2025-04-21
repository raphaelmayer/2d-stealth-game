#pragma once

#include "../components/Collider.hpp"
#include "../components/Interactable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../constants.hpp"
#include "../engine/types/Vec2i.hpp"
#include <easys/easys.hpp>
#include <string>

Easys::Entity instantiateSignEntity(Easys::ECS &ecs, Vec2i positionInTiles, const std::string &text = "")
{
	Easys::Entity npc = ecs.addEntity();

	ecs.addComponent(npc,
	                 Positionable{{(float)(positionInTiles.x * TILE_SIZE), (float)(positionInTiles.y * TILE_SIZE)}});
	ecs.addComponent(
	    npc, Renderable{SPRITE_SHEET, {5 * TILE_SIZE, 1 * TILE_SIZE}, {TILE_SIZE, TILE_SIZE}, {TILE_SIZE, TILE_SIZE}});
	ecs.addComponent(npc, Collider{});
	ecs.addComponent(npc, Interactable{text});

	return npc;
}