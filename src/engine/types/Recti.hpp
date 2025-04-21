#pragma once

#include <SDL.h>

struct Recti {
	int x, y, w, h;

	SDL_Rect toSDLRect() const { return {x, y, w, h}; }
};