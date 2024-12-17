#pragma once

struct Consumable {
	int health;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(health);
	}
};
