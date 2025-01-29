#pragma once

#include "../constants.hpp"
#include "LevelMap.hpp"
#include "MapLoader.hpp"
#include "TileRegistry.hpp"
#include <string>
#include <vector>

class MapManager {
  public:
	MapManager() { tileRegistry.loadFromFile(MAP_CHAR_MAPPING); }

	void loadMap(const int levelId)
	{
		levelMap = mapLoader.loadMap(levelId);
		printMap(levelMap);

		// create views
		walkableView = createWalkableMapView(levelMap);
	}

	const LevelMap &getLevelMap() const { return levelMap; }
	const TileRegistry &getTileRegistry() const { return tileRegistry; }
	const TileMetadata &getTileData(int id) const { return tileRegistry.getTileMetadata(id); }

	std::vector<std::vector<int>> createWalkableMapView(const LevelMap &map)
	{
		const Layer &backgroundLayer = map.getLayer(LayerID::BACKGROUND);
		const Layer &objectLayer = map.getLayer(LayerID::OBJECT);
		std::vector<std::vector<int>> walkableMapView;

		for (int y = 0; y < map.getHeight(); y++) {
			walkableMapView.push_back({});
			for (int x = 0; x < map.getWidth(); x++) {
				int tileIndex = Utils::to1d({x, y}, map.getWidth());
				TileMetadata backgroundData = getTileData(backgroundLayer[tileIndex]);
				TileMetadata objectData = getTileData(objectLayer[tileIndex]);

				if (objectData.id != 0)
					walkableMapView[y].push_back(!objectData.walkable);
				else
					walkableMapView[y].push_back(!backgroundData.walkable);
				std::cout << walkableMapView[y][x];
			}
			std::cout << std::endl;
		}
		return walkableMapView;
	}

	// for single entries, we could directly check. But i think decoupling everything from mapmanager makes sense.
	const std::vector<std::vector<int>> &getWalkableMapView() const { return walkableView; }
	int getWalkableMapView(int x, int y) const { return walkableView[y][x]; }

  private:
	void printMap(const LevelMap &map) const
	{
		int layerIndex = 0;
		for (auto &layerData : map.getLayers()) {
			std::cout << "Layer index: " << layerIndex++ << "\n";
			for (std::size_t i = 0; i < layerData.size(); i++) {
				std::cout << layerData[i] << " ";
				if ((i + 1) % map.getWidth() == 0)
					std::cout << "\n";
			}
			std::cout << "\n";
		}
	}

	TileRegistry tileRegistry;
	LevelMap levelMap = LevelMap(0, 0); // Initialize with dummy dimensions
	MapLoader mapLoader;

	// views
	std::vector<std::vector<int>> walkableView;
};