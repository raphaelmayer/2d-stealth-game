#pragma once

#include "../components/Controllable.hpp"
#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Engine.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/SaveGameManager.hpp"
#include "../ui/MenuStack.hpp"
#include "ConfirmationMenu.hpp"
#include "InventoryMenu.hpp"
#include "ListDialog.hpp"
#include "StatsMenu.hpp"
#include "TextDialog.hpp"

class InGameMenu final : public ListDialog {
  public:
	InGameMenu(Engine &game, ECSManager &ecs, GameStateManager &gameStateManager, SaveGameManager &saveGameManager,
	           MenuStack &menuStack)
	    : ListDialog(game, Vec2i{x, y}, menuWidth_), game_(game), ecs_(ecs), menuStack_(menuStack),
	      gameStateManager_(gameStateManager), saveGameManager_(saveGameManager)
	{
		setItems({{"ITEMS", [this, &game]() { pushMenu<InventoryMenu>(game); }},
		          {"STATS", [this, &game]() { pushMenu<StatsMenu>(game); }},
		          {"SAVE", [this, &game]() { confirmAndSave(game); }},
		          {"LOAD", [this, &game]() { confirmAndLoad(game); }},
		          {"FULLSCREEN", [&game]() { game.setWindowFullscreen(); }},
		          {"WINDOWED", [&game]() { game.setWindowWindowed(); }},
		          {"EXIT", [this, &game]() { confirmAndExit(game); }}});
	}

	void handleInput() override
	{
		if (game_.getKeyState(SDL_GetScancodeFromKey(SDLK_ESCAPE)).pressed && !openedThisFrame) {
			restorePlayerControl();
			menuStack_.pop();
		} else {
			ListDialog::handleInput();
		}
		openedThisFrame = false;
	}

  private:
	Engine &game_;
	ECSManager &ecs_;
	MenuStack &menuStack_;
	GameStateManager &gameStateManager_;
	SaveGameManager &saveGameManager_;
	bool openedThisFrame = true;

	static constexpr int margin = 20;
	static constexpr int menuWidth_ = 200;
	static constexpr int x = WINDOW_WIDTH * PIXEL_SIZE - menuWidth_ - margin;
	static constexpr int y = margin;

	template <typename MenuType, typename... Args>
	void pushMenu(Engine &game, Args &&...args)
	{
		menuStack_.push(std::make_unique<MenuType>(game, ecs_, menuStack_, std::forward<Args>(args)...));
	}

	void confirmAndSave(Engine &game)
	{
		auto saveAction = [this]() {
			saveGameManager_.save(SAVEFILE_PATH);
		};
		menuStack_.push(std::make_unique<ConfirmationMenu>(
		    game, menuStack_, "Are you sure you want to overwrite your save?", saveAction));
	}

	void confirmAndLoad(Engine &game)
	{
		auto loadAction = [this]() {
			saveGameManager_.load(SAVEFILE_PATH);
			menuStack_.pop(); // Return to previous menu
		};
		menuStack_.push(std::make_unique<ConfirmationMenu>(
		    game, menuStack_, "Are you sure? Any unsaved progress will be lost.", loadAction));
	}

	void confirmAndExit(Engine &game)
	{
		auto exitAction = [this]() {
			gameStateManager_.setGameState(GameState::MAINMENU);
			menuStack_.pop(); // Return to main menu
		};
		menuStack_.push(std::make_unique<ConfirmationMenu>(
		    game, menuStack_, "Are you sure? Any unsaved progress will be lost.", exitAction));
	}

	void restorePlayerControl() { ecs_.addComponent(PLAYER, Controllable{}); }
};
