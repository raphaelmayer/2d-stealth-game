#pragma once

#include <string>

struct Interactable {
	std::string text;
	bool isTextVisible = false;

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(text, false);
	}
};