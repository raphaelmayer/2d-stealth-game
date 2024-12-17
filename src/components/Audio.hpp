#pragma once

#include <string>

struct Audio {
	std::string file; 

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(file);
	}
};