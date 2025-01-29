#pragma once

#include "../constants.hpp"
#include "LevelMap.hpp"
#include <fstream>
#include <memory>
#include <string>
#include <tileson.hpp>
#include <vector>

class MapLoader {
  public:
	MapLoader() {}

	LevelMap loadMap(const int levelId)
	{
		tson::Tileson t;
		std::string path = "../assets/maps/01/map01.json"; // TODO: use levelId
		std::unique_ptr<tson::Map> tsonMap = t.parse(path);

		if (tsonMap->getStatus() != tson::ParseStatus::OK) {
			throw new std::runtime_error("Error loading map from: " + path);
		}

		const tson::Vector2i mapSize = tsonMap->getSize();
		const std::vector<tson::Layer> &layers = tsonMap->getLayers();
		LevelMap map(mapSize.x, mapSize.y);

		for (size_t layerIndex = 0; layerIndex < map.numLayers(); layerIndex++) {
			tson::Layer layer = layers[layerIndex];
			LayerID layerid = static_cast<LayerID>(layerIndex);
			loadLayer(layerid, layer, map);
		}

		return map;
	}

  private:
	void loadLayer(LayerID layerid, tson::Layer layer, LevelMap &map)
	{
		std::vector<unsigned int> layerUnsigned = layer.getData();
		std::vector<int> layerSigned(layerUnsigned.size());

		// Convert unsigned int vector to int vector, since tileson layers are unsigned
		std::transform(layerUnsigned.begin(), layerUnsigned.end(), layerSigned.begin(),
		               [](unsigned int id) { return static_cast<int>(id); });

		map.setLayer(layerid, layerSigned);
	}
};