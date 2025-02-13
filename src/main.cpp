#include "Game.hpp"
#include "./modules/StateMachine.hpp"

int main()
{

	//auto state = std::make_unique<NodeA>();
	//StateMachine sm(std::move(state));

	//sm.update();

	//sm.switchState(std::make_unique<NodeB>());
	// return 0;

	Game game = Game("Ants and Islands", {WINDOW_WIDTH, WINDOW_HEIGHT}, {PIXEL_SIZE, PIXEL_SIZE}, FPS);
	game.start();
	return 0;
}
