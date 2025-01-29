#pragma once

#include "TileRegistry.hpp"
#include <string>
#include <vector>

class LevelMap {
  public:
	LevelMap(int width, int height)
	    : width(width), height(height), backgroundLayer(height, std::vector<int>(width, 0)),
	      objectLayer(height, std::vector<int>(width, 0))
	{
	}

	void setBackgroundTile(int x, int y, int tile) { backgroundLayer[y][x] = tile; }
	void setObjectTile(int x, int y, int tile) { objectLayer[y][x] = tile; }
	void setTile(int x, int y, int bgTile, int objTile)
	{
		backgroundLayer[y][x] = bgTile;
		objectLayer[y][x] = objTile;
	}

	int getBackgroundTile(int x, int y) const { return backgroundLayer[y][x]; }
	int getObjectTile(int x, int y) const { return objectLayer[y][x]; }
	Tile getTile(int x, int y) const { return Tile(backgroundLayer[y][x], objectLayer[y][x]); }

	bool isInBounds(int x, int y) const { return x >= 0 && x < width && y >= 0 && y < height; }

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int size() const { return width * height; }

  private:
	int width, height;

	// tile layers
	std::vector<std::vector<int>> backgroundLayer{};
	std::vector<std::vector<int>> objectLayer{};
	std::vector<std::vector<int>> objectLayer2{};
	std::vector<std::vector<int>> foregroundLayer{}; // for perspective, i.e. sth character can move behind.
	std::vector<std::vector<int>> cosmeticLayer{};   // visual stuff. could be array of cosmetics layered on top?

	// property layers (we could use bit masks here but is it even necessary?)
	std::vector<std::vector<bool>> collisionLayer{};
	std::vector<std::vector<bool>> penetrationLayer{};
	std::vector<std::vector<bool>> visionLayer{};
};
