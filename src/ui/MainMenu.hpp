#pragma once

#include "../constants.hpp"
#include "../ecs/ECSManager.hpp"
#include "../engine/Engine.hpp"
#include "../engine/Vec2d.hpp"
#include "../modules/GameStateManager.hpp"
#include "../modules/MenuStack.hpp"
#include "../modules/SaveGameManager.hpp"
#include "ListDialog.hpp"

class MainMenu final : public ListDialog {
  public:
	MainMenu(Engine &game, GameStateManager &gameStateManager, SaveGameManager &saveGameManager, MenuStack &menuStack)
	    : ListDialog(game,
	                 {{"NEW GAME",
	                   [&gameStateManager, &saveGameManager, &menuStack]() {
		                   saveGameManager.load(WORLD_DEFINITION_PATH);
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
	                 Vec2d{x, y}, menuWidth_)
	{
	}

  private:
	static constexpr int menuWidth_ = 300;
	static constexpr int x = WINDOW_WIDTH * PIXEL_SIZE - menuWidth_ - 20;
	static constexpr int y = 218;
};
