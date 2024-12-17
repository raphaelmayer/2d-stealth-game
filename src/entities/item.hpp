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
#include "../ecs/ECSManager.hpp"
#include "../engine/Vec2d.hpp"

#define ITEMS_SPRITESHEET_Y 1000
#define SPRITE_SIZE TILE_SIZE

struct CollectableItem {
	Vec2d spriteSheetPos;
	Interactable interactable;
	Collectable collectable;
};

struct InteractableItem{
	Vec2d spriteSheetPos;
	Interactable interactable;
};

// instantiate item on map (not in some inventory) with tile values, not pixel values.
Entity instantiateItemEntity(ECSManager &ecs, const Vec2d position, const Vec2d spriteSheetPos)
{
	Entity item = ecs.addEntity();

	ecs.addComponent(item, Positionable{position * TILE_SIZE});
	ecs.addComponent(item,
	                 Renderable{{spriteSheetPos.x * SPRITE_SIZE, ITEMS_SPRITESHEET_Y + spriteSheetPos.y * SPRITE_SIZE},
	                            {SPRITE_SIZE, SPRITE_SIZE},
	                            0});
	ecs.addComponent(item, Collider{});

	return item;
}

Entity instantiateCollectableItemEntity(ECSManager &ecs, const Vec2d position, CollectableItem collectableItem)
{
	Entity item = instantiateItemEntity(ecs, position, collectableItem.spriteSheetPos);
	ecs.addComponent(item, std::move(collectableItem.interactable));
	ecs.addComponent(item, std::move(collectableItem.collectable));

	return item;
}

Entity instantiateInteractableItemEntity(ECSManager &ecs, const Vec2d position, InteractableItem interactableItem)
{
	Entity item = instantiateItemEntity(ecs, position, interactableItem.spriteSheetPos);
	ecs.addComponent(item, std::move(interactableItem.interactable));

	return item;
}