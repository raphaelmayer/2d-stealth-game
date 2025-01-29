#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// tile in the internal map, consisting of 2 layers of TileSprites, which are referenced by a char
// TODO: rename Tile -> TileInstance or TileComposition?
struct Tile {
	int backgroundId;
	int objectId;
};

struct CellMetadata {};

// Actual tile info
struct TileMetadata {
	int id = 0;
	std::string description;
	int spriteSheetX;
	int spriteSheetY;
	bool walkable;

	TileMetadata(int id, const std::string &desc, int x, int y, bool isWalkable)
	    : id(id), description(desc), spriteSheetX(x), spriteSheetY(y), walkable(isWalkable)
	{
	}

	TileMetadata() = default;
};

// new code to replace TileMetadata when we switch to TMX (Tiled)
struct TileMetadata_TBD {
	int id;                  // corresponds to 1d location in tileset
	std::string name;        // additional display name?
	std::string description; // necessary?

	// game-related tile parameters
	bool walkable;

	// TODO: add constructor for easy instantiation
};

class TileRegistry {
  public:
	void loadFromFile(const std::string &fileName)
	{
		std::ifstream file(fileName);
		if (!file.is_open()) {
			throw std::runtime_error("Unable to open tile registry file: " + fileName);
		}

		std::string line;
		std::getline(file, line); // Skip header
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			char id; // TODO: not used anymore. gotta remove all remains and rework overworld_char_mapping.csv.
			std::string description;
			int x, y;
			bool walkable;
			if (iss >> id >> description >> x >> y >> std::boolalpha >> walkable) {
				int tileid = y * (9) + x + 1; // should read 9 from tileset width
				tileMap.emplace(tileid, TileMetadata(tileid, description, x, y, walkable));
			} else {
				std::cerr << "Error parsing line: " << line << std::endl;
			}
		}
	}

	const TileMetadata &getTileMetadata(int id) const
	{
		auto it = tileMap.find(id);
		if (it == tileMap.end()) {
			return fallbackSprite;
		}
		return it->second;
	}

  private:
	std::map<int, TileMetadata> tileMap;
	TileMetadata fallbackSprite{0, "empty", 8, 0, true}; // Default fallback sprite
};