#pragma once

#include "../engine/types/Vec2i.hpp"
#include "TileRegistry.hpp"
#include <iostream>
#include <string>
#include <vector>

enum class LayerID {
	// tile layers
	BACKGROUND = 0,
	OBJECT,
	NUM_LAYERS,

	OBJECT2,
	FOREGROUND,
	COSMETIC,
	// property layers (we could use bit masks here but is it even necessary?)
	COLLISION,
	VISION,
	PENETRATION,
};

// struct Cell {
//	std::array<int, static_cast<size_t>(LayerID::NUM_LAYERS)> layers;
// };

using Layer = std::vector<std::vector<int>>;

class LevelMap {
  public:
	LevelMap(int width, int height) : width(width), height(height) {}

	void setTile(const Vec2i &position, LayerID layerid, int value)
	{
		layers[static_cast<size_t>(layerid)][position.y][position.x] = value;
	}

	int getTile(const Vec2i &position, LayerID layerid) const { return getLayer(layerid)[position.y][position.x]; }

	void setLayer(LayerID layerid, Layer layer) { layers[static_cast<size_t>(layerid)] = layer; }

	const Layer &getLayer(LayerID layerid) const { return layers[static_cast<size_t>(layerid)]; }

	bool isInBounds(int x, int y) const { return x >= 0 && x < width && y >= 0 && y < height; }

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int size() const { return width * height; }

  private:
	int width, height;
	std::array<Layer, static_cast<size_t>(LayerID::NUM_LAYERS)> layers;
};

// int main()
//{
//	LevelMap map(0, 0);
//
//	for (size_t layerid = 0; layerid < static_cast<size_t>(LayerID::NUM_LAYERS); layerid++) {
//		auto layer = map.getLayer(static_cast<LayerID>(layerid));
//		for (auto tile : layer) {
//			// map.setTile(position, layerid, tile);
//		}
//	}
//
//	Vec2i position;
//	int walkable = map.getTile(position, LayerID::COLLISION);
//	Layer walkableView = map.getLayer(LayerID::COLLISION);
//
//	return 0;
//}