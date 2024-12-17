#pragma once

#include <string>

struct Collectable {
	std::string name;
	std::string description;
	int armor;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(name, description, armor);
	}
};
