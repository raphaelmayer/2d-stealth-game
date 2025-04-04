#pragma once

#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Engine.hpp"
#include "../engine/types/Texture.hpp"
#include "../engine/types/Vec2i.hpp"
#include "../modules/GameStateManager.hpp"
#include "../modules/SaveGameManager.hpp"
#include "../ui/MenuStack.hpp"
#include "ListDialog.hpp"

class MainMenu final : public ListDialog {
  public:
	MainMenu(Engine &game, GameStateManager &gameStateManager, SaveGameManager &saveGameManager, MenuStack &menuStack)
	    : background(game.loadTexture(MAINMENU_BACKGROUND)), engine(game),
	      ListDialog(game,
	                 {{"NEW GAME",
	                   [&gameStateManager, &saveGameManager, &menuStack]() {
		                   //saveGameManager.load(WORLD_DEFINITION_PATH);
		                   gameStateManager.setGameState(GameState::PLAYING);
		                   menuStack.pop();
	                   }},
	                  {"LOAD",
	                   [&gameStateManager, &saveGameManager, &menuStack]() {
		                   saveGameManager.load(SAVEFILE_PATH);
		                   gameStateManager.setGameState(GameState::PLAYING);
		                   menuStack.pop();
	                   }},
	                  {"EXIT", [&game]() { game.stop(); }}},
	                 Vec2i{x, y}, menuWidth_)
	{
	}

	void render() override
	{
		engine.drawTexture(background);
		ListDialog::render();
	}

  private:
	const Engine &engine;
	const Texture background;
	static constexpr int menuWidth_ = 200;
	static constexpr int x = WINDOW_WIDTH * PIXEL_SIZE - menuWidth_;
	static constexpr int y = 700;
};
