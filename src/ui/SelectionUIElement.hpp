#pragma once

#include "../constants.hpp"
#include "../engine/Engine.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/SelectionManager.hpp"
#include "../ui/MenuStack.hpp"
#include <easys/easys.hpp>
#include <string>

class SelectionUIElement final : public UIElement {
  public:
	SelectionUIElement(Engine &game, Easys::ECS &ecs, const SelectionManager &selectionManager)
	    : UIElement(game), game_(game), ecs_(ecs), selectionManager_(selectionManager)
	{
	}

	void handleInput() override
	{
		// if (game_.getKeyState(/*LEFTMOUSEBUTTON*/).pressed) {
		//	// TODO: if multiple entities selected, select entity which was clicked on
		//	// needs to check mouse coordinates
		// }
	}

	void render() override
	{
		renderContainer(Vec2i{0, 0}, menuWidth_, 200);

		game_.setRenderScale({1, 1});

		// render
		if (selectionManager_.getSelection().empty()) {
			// do nothing.
		} else if (selectionManager_.getSelection().size() == 1) {
			renderSingle();
		} else if (selectionManager_.getSelection().size() > 1) {
			renderMultiple();
		}

		// game_.drawText(Recti{PADDING * 2, PADDING, menuWidth_, FONT_SIZE + 5}, "STATS");

		game_.setRenderScale({PIXEL_SIZE, PIXEL_SIZE});
	}

  private:
	Engine &game_;
	Easys::ECS &ecs_;
	const SelectionManager &selectionManager_;

	static constexpr int margin = 20;
	static constexpr int menuWidth_ = 350;
	static constexpr int x = WINDOW_WIDTH * PIXEL_SIZE - menuWidth_ - margin;
	static constexpr int y = margin;

	void renderSingle()
	{
		const std::vector<Easys::Entity> &selectedEntities = selectionManager_.getSelection();
		Easys::Entity entity = selectedEntities[0];
		EquippedWeapon weapon =
		    ecs_.hasComponent<EquippedWeapon>(entity) ? ecs_.getComponent<EquippedWeapon>(entity) : EquippedWeapon{};
		WeaponMetadata wdata = WeaponDatabase::getInstance().get(weapon.weaponId);

		std::string id = std::format("ID: {}", entity);
		std::string health = std::format("HEALTH: {}", ecs_.getComponent<Health>(entity).health);
		std::string wpn = std::format("WEAPON: {}", wdata.name);

		game_.drawText(Recti{PADDING * 2, PADDING, menuWidth_, FONT_SIZE + 5}, id);
		game_.drawText(Recti{PADDING * 2, PADDING + (FONT_SIZE + 5) * 1, menuWidth_, FONT_SIZE + 5}, health);
		game_.drawText(Recti{PADDING * 2, PADDING + (FONT_SIZE + 5) * 2, menuWidth_, FONT_SIZE + 5}, wpn);

		// display more info for player-controlled characters
		if (ecs_.hasComponent<Controllable>(entity)) {
			std::string ammo = std::format("AMMO: {}/{}", weapon.magazineSize, wdata.magazineSize);
			game_.drawText(Recti{PADDING * 2, PADDING + (FONT_SIZE + 5) * 3, menuWidth_, FONT_SIZE + 5}, ammo);
		}
	}

	void renderMultiple()
	{
		const std::vector<Easys::Entity> &selectedEntities = selectionManager_.getSelection();
		std::string entitiesStr;
		for (const auto &entity : selectedEntities) {
			if (!entitiesStr.empty()) {
				entitiesStr.append(",");
			}
			entitiesStr.append(std::to_string(entity));
		}
		game_.drawText(Recti{PADDING * 2, PADDING, menuWidth_, FONT_SIZE + 5}, entitiesStr);
	}
};