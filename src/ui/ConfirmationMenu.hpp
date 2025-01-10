#pragma once

#include "../constants.hpp"
#include "../modules/GameStateManager.hpp"
#include "../modules/MenuStack.hpp"
#include "../modules/SaveGameManager.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Engine.hpp"
#include "../engine/Vec2i.hpp"
#include "ListDialog.hpp"
#include "TextDialog.hpp"
#include "UIElement.hpp"
#include <string>

// A UI component that combines a DialogueBox with a ListMenu for binary ('Yes'/'No') user confirmations.
class ConfirmationMenu final : public UIElement {
  public:
	ConfirmationMenu(Engine &game, MenuStack &menuStack, std::string text, std::function<void()> action)
	    : UIElement(game), game_(game), menuStack_(menuStack), textBox(game, menuStack, text), listMenu(game,
	                                                                    {{"YES",
	                                                                      [&menuStack, action]() {
		                                                                      action();
		                                                                      menuStack.pop();
	                                                                      }},
	                                                                     {"NO", [&menuStack]() { menuStack.pop(); }}},
	                                                                    Vec2i{x, y}, menuWidth_)

	{
	}

	void handleInput() override
	{
		if (game_.getKeyState(SDL_GetScancodeFromKey(SDLK_ESCAPE)).pressed)
			menuStack_.pop();
		else
			listMenu.handleInput();
	}

	void render() override
	{
		textBox.render();
		listMenu.render();
	}

  private:
	Engine &game_;
	MenuStack &menuStack_;
	TextDialog textBox;
	ListDialog listMenu;

	static constexpr int menuWidth_ = 100;
	static constexpr int x = WINDOW_WIDTH * PIXEL_SIZE - menuWidth_ - 30;
	static constexpr int y = 160;
};
