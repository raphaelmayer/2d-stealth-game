#pragma once

#include "../constants.hpp"
#include "LevelMap_NEW.hpp"
#include "TileRegistry.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <tileson.hpp>
#include <vector>

class MapManager {
  public:
	MapManager() { tileRegistry.loadFromFile(MAP_CHAR_MAPPING); }

	void loadMap(const int levelId)
	{
		tson::Tileson t;
		auto path = "../assets/maps/01/map01.json"; // TODO: use levelId
		std::unique_ptr<tson::Map> map = t.parse(path);

		if (map->getStatus() != tson::ParseStatus::OK) {
			// TODO
			std::cout << "Error loading map \n";
			return;
		}
		tson::Vector2i mapSize = map->getSize();
		levelMap = LevelMap(mapSize.x, mapSize.y);

		const auto &layers = map->getLayers();

		int layerIndex = 0;
		for (size_t layerIndex = 0; layerIndex < static_cast<size_t>(LayerID::NUM_LAYERS); layerIndex++) {
		//for (const tson::Layer &layer : layers) {
			LayerID layerid = static_cast<LayerID>(layerIndex);
			auto layer = layers[layerIndex];
			auto layer1d = layer.getData();
			std::vector<std::vector<int>> layer2d;
			for (size_t y = 0; y < mapSize.y; y++) {
				layer2d.push_back({});
				for (size_t x = 0; x < mapSize.x; x++) {
					layer2d[y].push_back(layer1d[y * mapSize.x + x]);
				}
			}
			levelMap.setLayer(layerid, layer2d);
		}

		for (size_t i = 0; i < static_cast<size_t>(LayerID::NUM_LAYERS); i++) {
			std::cout << "layer index: " << i << "\n";
			for (int y = 0; y < levelMap.getHeight(); y++) {
				for (int x = 0; x < levelMap.getWidth(); x++) {
					std::cout << levelMap.getTile({x, y}, static_cast<LayerID>(i)) << " ";
				}
				std::cout << "\n";
			}
		}

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
				// int tileid = y * (9) + x + 1;
				TileMetadata backgroundData = getTileData(backgroundLayer[y][x]);
				TileMetadata objectData = getTileData(objectLayer[y][x]);

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
	// int getWalkableMapView(int x, int y) const { return getTileData(x, y)[1].walkable; }
	//  void updateWalkableMapView(std::vector<std::vector<int>>) {}
	//  void updateWalkableMapView(int x, int y, int value) {}

  private:
	TileRegistry tileRegistry;
	LevelMap levelMap = LevelMap(0, 0); // Initialize with dummy dimensions

	// views
	std::vector<std::vector<int>> walkableView;
};