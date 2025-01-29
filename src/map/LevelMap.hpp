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
	OBJECT2,
	COSMETIC,
	FOREGROUND,
	NUM_LAYERS
};

using Layer = std::vector<int>;

class LevelMap {
  public:
	LevelMap(int width, int height) : width(width), height(height) {}

	void setTile(LayerID layerid, int index, int value) { getLayerNonConst(layerid)[index] = value; }
	int getTile(LayerID layerid, int index) const { return getLayer(layerid)[index]; }

	void setLayer(LayerID layerid, std::vector<int> layer) { getLayerNonConst(layerid) = layer; }
	const Layer &getLayer(LayerID layerid) const { return layers[static_cast<size_t>(layerid)]; }
	const std::array<Layer, static_cast<size_t>(LayerID::NUM_LAYERS)> &getLayers() const { return layers; }

	bool isInBounds(int x, int y) const { return x >= 0 && x < width && y >= 0 && y < height; }

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	int size() const { return width * height; }
	int numLayers() const { return static_cast<int>(LayerID::NUM_LAYERS); }

  private:
	Layer &getLayerNonConst(LayerID layerid) { return layers[static_cast<size_t>(layerid)]; }
	
	int width, height;
	std::array<Layer, static_cast<size_t>(LayerID::NUM_LAYERS)> layers;
};