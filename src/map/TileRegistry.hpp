#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// Tile properties.
// `id` represents the tile's index on the spritesheet, offset by +1.
// This is because in Tiled, tile IDs start from 1 — ID 0 is reserved for "no tile" (i.e., empty space).
// However, *when selecting tiles in the Tiled editor*, it displays 0-based indices,
// which can be confusing — the visual index you see is one less than the actual ID used in data.
struct TileMetadata {
	int id = 0;
	bool walkable;
};

class TileRegistry : public CSVDatabase<int, TileMetadata> {
  public:
	TileRegistry() : CSVDatabase(TILE_PROPERTIES, parseRow)
	{
	}
	const TileMetadata &getTileMetadata(int id) const
	{
		try {
			return get(id);
		} catch (std::out_of_range err) {
			// TODO: we might want to handle this differently. either notify user of invalid key or return visually
			// obvious fallback sprite
			return fallbackSprite;
		}
	}

  private:
	TileMetadata fallbackSprite{0, true}; // Default fallback sprite

	static std::pair<int, TileMetadata> parseRow(const std::vector<std::string> &tokens)
	{
		return {std::stoi(tokens[0]), TileMetadata{std::stoi(tokens[0]), std::stoi(tokens[1]) == 1}};
	}
};