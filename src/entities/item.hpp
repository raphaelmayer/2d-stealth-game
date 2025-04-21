#pragma once

#include <utility>

#include "../components/Collectable.hpp"
#include "../components/Collider.hpp"
#include "../components/Consumable.hpp"
#include "../components/Controllable.hpp"
#include "../components/Interactable.hpp"
#include "../components/Positionable.hpp"
#include "../components/Renderable.hpp"
#include "../components/RigidBody.hpp"
#include "../constants.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/Utils.hpp"
#include <easys/easys.hpp>

#define ITEMS_SPRITESHEET_Y 1000
#define SPRITE_SIZE TILE_SIZE

struct CollectableItem {
	Vec2i spriteSheetPos;
	Interactable interactable;
	Collectable collectable;
};

struct InteractableItem {
	Vec2i spriteSheetPos;
	Interactable interactable;
};

// instantiate item on map (not in some inventory) with tile values, not pixel values.
Easys::Entity instantiateItemEntity(Easys::ECS &ecs, const Vec2i position, const Vec2i spriteSheetPos)
{
	Easys::Entity item = ecs.addEntity();

	ecs.addComponent(item, Positionable{Utils::toFloat(position) * TILE_SIZE});
	ecs.addComponent(item,
	                 Renderable{SPRITE_SHEET,
	                            {spriteSheetPos.x * SPRITE_SIZE, ITEMS_SPRITESHEET_Y + spriteSheetPos.y * SPRITE_SIZE},
	                            {SPRITE_SIZE, SPRITE_SIZE},
	                            {SPRITE_SIZE, SPRITE_SIZE}});
	ecs.addComponent(item, Collider{});

	return item;
}

Easys::Entity instantiateCollectableItemEntity(Easys::ECS &ecs, const Vec2i position, CollectableItem collectableItem)
{
	Easys::Entity item = instantiateItemEntity(ecs, position, collectableItem.spriteSheetPos);
	ecs.addComponent(item, std::move(collectableItem.interactable));
	ecs.addComponent(item, std::move(collectableItem.collectable));

	return item;
}

Easys::Entity instantiateInteractableItemEntity(Easys::ECS &ecs, const Vec2i position, InteractableItem interactableItem)
{
	Easys::Entity item = instantiateItemEntity(ecs, position, interactableItem.spriteSheetPos);
	ecs.addComponent(item, std::move(interactableItem.interactable));

	return item;
}