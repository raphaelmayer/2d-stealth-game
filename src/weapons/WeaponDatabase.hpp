#pragma once

#include "../modules/CSVDatabase.hpp"
#include "WeaponMetadata.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>

using WeaponID = int;

class WeaponDatabase : public CSVDatabase<WeaponID, WeaponMetadata> {
  public:
	WeaponDatabase(const std::string &filePath)
	    : CSVDatabase<WeaponID, WeaponMetadata>(filePath, parseWeapon)
	{
		auto w = get(1);
		std::cout << "DB Size: " << size() << "\n";
		std::cout << "Weapon name: " << w.name << "\n";
	}

  private:
	static std::pair<WeaponID, WeaponMetadata> parseWeapon(const std::vector<std::string> &tokens)
	{
		if (tokens.size() < 9) { // Ensure we have enough fields.
			throw std::runtime_error("Not enough fields in line.");
		}
		WeaponMetadata wdata;
		try {
			wdata.id = std::stoi(tokens[0]);
			wdata.name = tokens[1];
			wdata.description = tokens[2];
			wdata.velocity = std::stof(tokens[3]);
			wdata.range = std::stof(tokens[4]);
			wdata.damage = std::stoi(tokens[5]);
			wdata.warmup = std::stoi(tokens[6]);
			wdata.firerate = std::stoi(tokens[7]);
			wdata.magazineSize = std::stoi(tokens[8]);
		} catch (const std::invalid_argument &e) {
			throw std::runtime_error("Invalid data format: " + std::string(e.what()));
		} catch (const std::out_of_range &e) {
			throw std::runtime_error("Data out of range: " + std::string(e.what()));
		}
		return {wdata.id, wdata};
	}
};
