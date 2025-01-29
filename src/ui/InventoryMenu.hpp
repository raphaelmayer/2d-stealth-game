#pragma once

#include "../components/Collectable.hpp"
#include "../components/Consumable.hpp"
#include "../components/Controllable.hpp"
#include "../components/Inventory.hpp"
#include "../components/Renderable.hpp"
#include "../components/Stats.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Engine.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/SaveGameManager.hpp"
#include "../ui/MenuStack.hpp"
#include "ListDialog.hpp"
#include "ListItem.hpp"
#include "TextDialog.hpp"
#include <vector>

class InventoryMenu final : public UIElement {
  public:
	InventoryMenu(Engine &game, ECSManager &ecs, MenuStack &menuStack)
	    : UIElement(game), game_(game), ecs_(ecs), menuStack_(menuStack), listMenu_(game, Vec2i{x, y}, menuWidth_, 6),
	      textBox_(game, menuStack)
	{
		updateInventoryList();
	}

	void handleInput() override
	{
		if (game_.getKeyState(SDL_GetScancodeFromKey(SDLK_ESCAPE)).pressed) {
			menuStack_.pop();
		} else {
			updateDescription();
			listMenu_.handleInput();
		}
	}

	void render() override
	{
		listMenu_.render();
		textBox_.render();
	}

  private:
	Engine &game_;
	ECSManager &ecs_;
	MenuStack &menuStack_;
	ListDialog listMenu_;
	TextDialog textBox_;

	static constexpr int menuWidth_ = 200;
	static constexpr int x = WINDOW_WIDTH * PIXEL_SIZE - menuWidth_ - 30;
	static constexpr int y = 30;

	void updateInventoryList()
	{
		std::vector<Entity> &inventory = ecs_.getComponent<Inventory>(PLAYER).items;
		std::vector<ListItem> items;

		for (auto &itemEntity : inventory) {
			auto item = ecs_.getComponent<Collectable>(itemEntity);
			items.push_back(createListItemForEntity(itemEntity, item));
		}

		items.push_back({"CANCEL", [this]() { menuStack_.pop(); }});
		listMenu_.setItems(items);
	}

	ListItem createListItemForEntity(Entity itemEntity, const Collectable &item)
	{
		return {item.name, [this, itemEntity]() {
			        applyEffectsIfConsumable(itemEntity);
			        updateInventoryList();
		        }};
	}

	void applyEffectsIfConsumable(Entity itemEntity)
	{
		auto &collectable = ecs_.getComponent<Collectable>(itemEntity);

		if (!ecs_.hasComponent<Consumable>(itemEntity)) {
			pushTextDialog("You cannot consume " + collectable.name + ".");
			return;
		}

		auto &playerStats = ecs_.getComponent<Stats>(PLAYER);
		auto &consumable = ecs_.getComponent<Consumable>(itemEntity);

		playerStats.health += consumable.health;
		pushTextDialog("You have consumed " + collectable.name + ".");

		// remove item
		auto &inventory = ecs_.getComponent<Inventory>(PLAYER).items;
		auto it = std::find(inventory.begin(), inventory.end(), itemEntity);
		if (it != inventory.end()) {
			inventory.erase(it);
		}
		ecs_.removeEntity(itemEntity);
	}

	void updateDescription()
	{
		size_t index = listMenu_.getIndex();
		std::vector<Entity> &items = ecs_.getComponent<Inventory>(PLAYER).items;

		// we check against the original inventory to see, if we are on an item or the cancel button
		if (index < items.size()) {
			std::string desc = ecs_.getComponent<Collectable>(items[index]).description;
			textBox_.setText(desc);
		} else {
			textBox_.setText("No items in inventory.");
		}
	}

	void pushTextDialog(std::string text) { menuStack_.push<TextDialog>(game_, menuStack_, text); }
};
