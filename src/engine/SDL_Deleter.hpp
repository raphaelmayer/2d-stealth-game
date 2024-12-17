#include <SDL.h>
#include <SDL_ttf.h>

// required for wrapping SDL pointers with std::unique_ptr
struct SDL_Deleter {
	void operator()(SDL_Window *window) { SDL_DestroyWindow(window); }

	void operator()(SDL_Renderer *renderer) { SDL_DestroyRenderer(renderer); }

	void operator()(TTF_Font *font) { TTF_CloseFont(font); }
};