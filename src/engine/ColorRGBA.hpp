#pragma once

#include <SDL.h> // for Uint8 definition

struct ColorRGBA {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;

	ColorRGBA(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha = 255) : r(red), g(green), b(blue), a(alpha) {}
};