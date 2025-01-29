#include "Game.hpp"
#include "constants.hpp"
// #include "map/MapManagerTMX.hpp"

int main()
{
	/*MapManager mm;

	mm.loadMap(1);*/

	Game game = Game("Ants and Islands", {WINDOW_WIDTH, WINDOW_HEIGHT}, {PIXEL_SIZE, PIXEL_SIZE}, FPS);
	game.start();
	return 0;
}
