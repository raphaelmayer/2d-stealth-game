#pragma once

#include "../components/Animatable.hpp"
#include "../components/Collider.hpp"
#include "../components/Controllable.hpp"
#include "../components/EquippedWeapon.hpp"
#include "../components/Health.hpp"
#include "../components/Interactable.hpp"
#include "../components/Inventory.hpp"
#include "../components/Pathfinding.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../components/Rotatable.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/types/Vec2i.hpp"
#include "entity.hpp"

// for playerSpriteSheetY use one of the constants PLAYER_{RED|BLUE|WHITE}_SPRITE_SHEET_Y
Entity instantiatePlayerEntity(ECSManager &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH,
                               int playerSpriteSheetY = PLAYER_SPRITE_SHEET_Y_RED)
{
	Entity player = instantiateBaseEntity(ecs, positionInTiles, rotation);

	ecs.addComponent(player, Controllable{});
	ecs.addComponent(player, Inventory{});
	ecs.addComponent(player, EquippedWeapon{1, 30}); // assault rifle with full mag
	ecs.addComponent(player, Pathfinding{});

	return player;
}