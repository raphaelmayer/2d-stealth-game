#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// Actual tile info
struct TileMetadata {
	int id = 0;
	bool walkable;

	TileMetadata(int id, bool isWalkable)
	    : id(id), walkable(isWalkable)
	{
	}

	TileMetadata() = default;
};

// TODO: new struct to replace TileMetadata when we switch to a new tileset
struct TileMetadata_TBD {
	int id;                  // corresponds to 1d location in tileset
	std::string name;        // additional display name?
	std::string description; // necessary?

	// game-related tile parameters
	bool walkable;

	// TODO: add constructor for easy instantiation
};

// TODO: Refactor to use CSVDatabase class, as soon as we switch to the new tileset.
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
			std::string idStr, walkableStr;

			if (std::getline(iss, idStr, ',') && std::getline(iss, walkableStr)) {
				try {
					int id = std::stoi(idStr);
					bool walkable = std::stoi(walkableStr) == 1;
					tileMap.emplace(id, TileMetadata(id, walkable));
				} catch (const std::exception &e) {
					std::cerr << "Conversion error in line: " << line << " (" << e.what() << ")\n";
				}
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
	TileMetadata fallbackSprite{0, true}; // Default fallback sprite
};