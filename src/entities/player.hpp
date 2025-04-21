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
#include "../engine/types/Vec2i.hpp"
#include "character.hpp"
#include <easys/easys.hpp>

Easys::Entity instantiatePlayerEntity(Easys::ECS &ecs, Vec2i positionInTiles, Rotation rotation = SOUTH)
{
	Easys::Entity player = instantiateBaseCharacter(ecs, positionInTiles, rotation);

	ecs.addComponent(player, Controllable{});
	ecs.addComponent(player, Inventory{});
	ecs.addComponent(player, EquippedWeapon{1, 30}); // assault rifle with full mag
	ecs.addComponent(player, Pathfinding{});

	return player;
}