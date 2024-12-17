#pragma once

struct Stats {
	unsigned health;
	unsigned armor;
	unsigned experience;
	unsigned progress;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(health, armor, experience, progress);
	}
};