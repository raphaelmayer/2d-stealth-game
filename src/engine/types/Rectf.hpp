#pragma once

#include <SDL.h>

struct Rectf {
	float x, y, w, h;

	SDL_FRect toSDLRect() const { return {x, y, w, h}; }
};