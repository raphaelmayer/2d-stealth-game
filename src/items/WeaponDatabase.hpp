#pragma once

#include "../constants.hpp"
#include "../modules/CSVDatabase.hpp"
#include "WeaponMetadata.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

// Implemented as a singleton for easier access. These are only ever read from so i think this is a good choice.
class WeaponDatabase : public CSVDatabase<WeaponID, WeaponMetadata> {
  public:
	// Delete copy constructor and assignment operator to prevent copies
	WeaponDatabase(const WeaponDatabase &) = delete;
	WeaponDatabase &operator=(const WeaponDatabase &) = delete;

	// Singleton accessor
	static WeaponDatabase &getInstance(const std::string &filePath = WEAPONDATA_PATH)
	{
		static WeaponDatabase instance(filePath);
		return instance;
	}

  private:
	// Private constructor to enforce singleton pattern
	WeaponDatabase(const std::string &filePath) : CSVDatabase<WeaponID, WeaponMetadata>(filePath, parseWeapon) {}

	static std::pair<WeaponID, WeaponMetadata> parseWeapon(const std::vector<std::string> &tokens)
	{
		if (tokens.size() < 9) { // Ensure we have enough fields.
			throw std::runtime_error("Not enough fields in line.");
		}
		WeaponMetadata wdata;
		try {
			wdata.id = static_cast<WeaponID>(std::stoi(tokens[0]));
			wdata.name = tokens[1];
			wdata.description = tokens[2];
			wdata.speed = std::stof(tokens[3]);
			wdata.range = std::stof(tokens[4]);
			wdata.damage = std::stoi(tokens[5]);
			wdata.warmup = std::stof(tokens[6]);
			wdata.firerate = std::stof(tokens[7]);
			wdata.magazineSize = std::stoi(tokens[8]);
			wdata.reloadTime = std::stof(tokens[9]);
			wdata.spread = std::stof(tokens[10]);
		} catch (const std::invalid_argument &e) {
			throw std::runtime_error("Invalid data format: " + std::string(e.what()));
		} catch (const std::out_of_range &e) {
			throw std::runtime_error("Data out of range: " + std::string(e.what()));
		}
		return {wdata.id, wdata};
	}
};
