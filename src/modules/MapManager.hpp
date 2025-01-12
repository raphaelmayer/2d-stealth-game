#pragma once

#include "../constants.hpp"
#include "LevelMap.hpp"
#include "TileRegistry.hpp"
#include <fstream>
#include <string>
#include <vector>

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
		// int sizeY = static_cast<int>(background.size());
		// int sizeX = static_cast<int>(background[0].size());
		levelMap = LevelMap(MAP_SIZE_X, MAP_SIZE_Y);

		for (int y = 0; y < MAP_SIZE_Y; ++y) {
			for (int x = 0; x < MAP_SIZE_X; ++x) {
				levelMap.setBackgroundTile(x, y, background[y][x]);
				levelMap.setObjectTile(x, y, objects[y][x]);
			}
		}

		// create views
		walkableView = createWalkableMapView();
	}

	const LevelMap &getLevelMap() const { return levelMap; }
	const TileRegistry &getTileRegistry() const { return tileRegistry; }
	const Tile getTile(int x, int y) const { return levelMap.getTile(x, y); }

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

	std::vector<std::vector<int>> createWalkableMapView()
	{
		std::vector<std::vector<int>> walkableMapView;
		for (int y = 0; y < MAP_SIZE_Y; y++) {
			walkableMapView.push_back({});
			for (int x = 0; x < MAP_SIZE_X; x++) {
				const std::vector<TileMetadata> fullTiledata = getTileData(x, y);
				if (fullTiledata[1].walkable)
					walkableMapView[y].push_back(0);
				else
					walkableMapView[y].push_back(1);
				std::cout << walkableMapView[y][x];
			}
			std::cout << std::endl;
		}
		return walkableMapView;
	}

	// for single entries, we could directly check. But i think decoupling everything from mapmanager makes sense.
	const std::vector<std::vector<int>> &getWalkableMapView() const { return walkableView; }
	int getWalkableMapView(int x, int y) const { return walkableView[y][x]; }
	// int getWalkableMapView(int x, int y) const { return getTileData(x, y)[1].walkable; }
	//  void updateWalkableMapView(std::vector<std::vector<int>>) {}
	//  void updateWalkableMapView(int x, int y, int value) {}

  private:
	TileRegistry tileRegistry;
	LevelMap levelMap = LevelMap(0, 0); // Initialize with dummy dimensions

	// views
	std::vector<std::vector<int>> walkableView;
};