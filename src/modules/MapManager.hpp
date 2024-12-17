#pragma once

#include <vector>
#include <fstream>
#include <string>
#include "LevelMap.hpp"
#include "TileRegistry.hpp"
#include "../constants.hpp"

class MapManager {
  public:
	MapManager() { tileRegistry.loadFromFile(MAP_CHAR_MAPPING); }

	void loadMap(const int levelId)
	{
		// TODO: use levelId

		std::ifstream bgFile(MAP_BACKGROUND);
		std::ifstream objFile(MAP_OBJECTS);

		if (!bgFile.is_open() || !objFile.is_open()) {
			throw std::runtime_error("Unable to open map files");
		}

		std::vector<std::vector<char>> background, objects;

		std::string bgLine, objLine;
		while (std::getline(bgFile, bgLine) && std::getline(objFile, objLine)) {
			std::vector<char> bgRow(bgLine.begin(), bgLine.end());
			std::vector<char> objRow(objLine.begin(), objLine.end());
			background.push_back(bgRow);
			objects.push_back(objRow);
		}
		// TOOD: read height and width from map file
		int sizeY = background.size();
		int sizeX = background[0].size();
		levelMap = LevelMap(MAP_SIZE_X, MAP_SIZE_Y);

		for (int y = 0; y < MAP_SIZE_Y; ++y) {
			for (int x = 0; x < MAP_SIZE_X; ++x) {
				levelMap.setBackgroundTile(x, y, background[y][x]);
				levelMap.setObjectTile(x, y, objects[y][x]);
			}
		}
	}

	const LevelMap &getLevelMap() const { return levelMap; }
	const TileRegistry &getTileRegistry() const { return tileRegistry; }
	const Tile &getTile(int x, int y) const { return levelMap.getTile(x, y); }

	const std::vector<TileMetadata> getTileData(Tile tile) const
	{
		const TileMetadata &bgdata = tileRegistry.getTileMetadata(tile.backgroundId);
		const TileMetadata &objdata = tileRegistry.getTileMetadata(tile.objectId);
		return {bgdata, objdata};
	}

	const std::vector<TileMetadata> getTileData(int x, int y) const
	{
		Tile tile = levelMap.getTile(x, y);
		return getTileData(tile);
	}
	
	const TileMetadata &getTileData(char id) const { return tileRegistry.getTileMetadata(id); }

  private:
	TileRegistry tileRegistry;
	LevelMap levelMap = LevelMap(0, 0); // Initialize with dummy dimensions
};