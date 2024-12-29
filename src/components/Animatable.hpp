#pragma once
#include "../constants.hpp"

struct Animatable {
	int currentAnimation;
	int animationAdresses[PLAYER_NUMBER_ANIMATIONS];
	double timeElapsed = 0; // accumulator for knowing where in the animation cycle we are

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(currentAnimation, animationAdresses);
	}
};