#pragma once

enum GameState {
	// STARTMENU,
	MAINMENU,
	// LOADING,
	PLAYING
};

class GameStateManager {
  public:
	void setGameState(const GameState gameState) { gameState_ = gameState; }
	GameState getGameState() const { return gameState_; }

  private:
	GameState gameState_ = GameState::MAINMENU;
};