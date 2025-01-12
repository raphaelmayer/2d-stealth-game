#pragma once

#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Engine.hpp"
#include "../engine/Vec2i.hpp"
#include "../modules/MenuStack.hpp"
#include <string>

class StatsMenu final : public UIElement {
  public:
	StatsMenu(Engine &game, ECSManager &ecs, MenuStack &menuStack)
	    : UIElement(game), game_(game), menuStack_(menuStack), stats_(ecs.getComponent<Stats>(PLAYER))
	{
	}

	void handleInput() override
	{
		if (game_.getKeyState(SDL_SCANCODE_ESCAPE).pressed || game_.getKeyState(SDL_SCANCODE_RETURN).pressed)
			menuStack_.pop();
	}

	void render() override
	{
		renderContainer(Vec2i{0, 0}, menuWidth_, 200);

		game_.setRenderScale({1, 1});

		game_.drawText(Recti{PADDING * 2, PADDING, menuWidth_, FONT_SIZE + 5}, "STATS");
		game_.drawText(Recti{PADDING * 2, FONT_SIZE * 1 + PADDING, menuWidth_, FONT_SIZE},
		               "Health: " + std::to_string(stats_.health));
		game_.drawText(Recti{PADDING * 2, FONT_SIZE * 2 + PADDING, menuWidth_, FONT_SIZE},
		               "Armor: " + std::to_string(stats_.armor + inventoryArmor_));
		game_.drawText(Recti{PADDING * 2, FONT_SIZE * 3 + PADDING, menuWidth_, FONT_SIZE},
		               "Experience: " + std::to_string(stats_.experience));
		game_.drawText(Recti{PADDING * 2, FONT_SIZE * 4 + PADDING, menuWidth_, FONT_SIZE},
		               "Progress: " + std::to_string((int)((float)stats_.progress / PROGRESS_STEPS * 100)) + "%");

		game_.setRenderScale({PIXEL_SIZE, PIXEL_SIZE});
	}

  private:
	Engine &game_;
	MenuStack &menuStack_;
	Stats stats_;
	unsigned inventoryArmor_ = 0; // crude way to sum up armor pieces in inventory

	static constexpr int margin = 20;
	static constexpr int menuWidth_ = 350;
	static constexpr int x = WINDOW_WIDTH * PIXEL_SIZE - menuWidth_ - margin;
	static constexpr int y = margin;
};