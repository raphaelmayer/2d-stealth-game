#pragma once

#include<SDL.h>
#include <stdexcept>
#include <string>

class Texture {
  public:
	Texture(SDL_Texture *texture) { texture_ = texture; }

	Texture(SDL_Renderer *renderer, const std::string &path) : texture_(nullptr)
	{
		if (!renderer) {
			throw std::invalid_argument("Renderer must not be null.");
		}

		SDL_Surface *surface = SDL_LoadBMP(path.c_str()); // Replace with SDL_Image for PNG/JPG if needed
		if (!surface) {
			throw std::runtime_error("Failed to load texture: " + path + ", SDL Error: " + SDL_GetError());
		}

		texture_ = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		if (!texture_) {
			throw std::runtime_error("Failed to create texture from surface: " + path
			                         + ", SDL Error: " + SDL_GetError());
		}
	}

	~Texture()
	{
		if (texture_) {
			SDL_DestroyTexture(texture_);
		}
	}

	// Non-copyable
	Texture(const Texture &) = delete;
	Texture &operator=(const Texture &) = delete;

	// Movable
	Texture(Texture &&other) noexcept : texture_(other.texture_) { other.texture_ = nullptr; }

	bool operator!() const { return !texture_; }

	Texture &operator=(Texture &&other) noexcept
	{
		if (this != &other) {
			if (texture_) {
				SDL_DestroyTexture(texture_);
			}
			texture_ = other.texture_;
			other.texture_ = nullptr;
		}
		return *this;
	}

	SDL_Texture *getSDLTexture() const { return texture_; }

  private:
	SDL_Texture *texture_; // Internal SDL texture
};
