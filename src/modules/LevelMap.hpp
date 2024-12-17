#pragma once

#include "TileRegistry.hpp"
#include <string>
#include <vector>

class LevelMap {
  public:
	LevelMap(int width, int height)
	    : width(width), height(height), backgroundLayer(height, std::vector<char>(width, ' ')),
	      objectLayer(height, std::vector<char>(width, ' '))
	{
	}

	void setBackgroundTile(int x, int y, char tile) { backgroundLayer[y][x] = tile; }
	void setObjectTile(int x, int y, char tile) { objectLayer[y][x] = tile; }
	void setTile(int x, int y, char bgTile, char objTile)
	{
		backgroundLayer[y][x] = bgTile;
		objectLayer[y][x] = objTile;
	}

	char getBackgroundTile(int x, int y) const { return backgroundLayer[y][x]; }
	char getObjectTile(int x, int y) const { return objectLayer[y][x]; }
	Tile getTile(int x, int y) const { return Tile(backgroundLayer[y][x], objectLayer[y][x]); }

	bool isInBounds(int x, int y) const { return x >= 0 && x < width && y >= 0 && y < height; }

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int size() const { return width * height; }

  private:
	int width, height;
	std::vector<std::vector<char>> backgroundLayer;
	std::vector<std::vector<char>> objectLayer;
};
