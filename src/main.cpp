#include "Game.hpp"
#include "constants.hpp"

int main()
{
	Game game = Game("Ants and Islands", {WINDOW_WIDTH, WINDOW_HEIGHT}, {PIXEL_SIZE, PIXEL_SIZE}, FPS);
	game.start();
	return 0;
}
