#pragma once
#include "../constants.hpp"

struct Animatable {
	int currentAnimation;
	int animationAdresses[PLAYER_NUMBER_ANIMATIONS];

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(currentAnimation, animationAdresses);
	}
};