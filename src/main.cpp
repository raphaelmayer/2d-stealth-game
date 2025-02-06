// #include "Game.hpp"
#include "constants.hpp"
#include "modules/CSVParser.hpp"
#include "weapons/WeaponDatabase.hpp"
#include "weapons/WeaponMetadata.hpp"

int main()
{
	WeaponDatabase wdb("../assets/weapon_data.csv");
	return 0;

	// Game game = Game("Ants and Islands", {WINDOW_WIDTH, WINDOW_HEIGHT}, {PIXEL_SIZE, PIXEL_SIZE}, FPS);
	// game.start();
	// return 0;
}
