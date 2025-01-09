#pragma once

#define SDL_MAIN_HANDLED

#include "../constants.hpp"
#include "ColorRGBA.hpp"
#include "FPSCounter.hpp"
#include "FrameRateLimiter.hpp"
#include "FrameTimer.hpp"
#include "Keyboard.hpp"
#include "KeyState.hpp"
#include "Mouse.hpp"
#include "SDL_Deleter.hpp"
#include "Vec2d.hpp"
#include <array>
#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class Engine {
  public:
	Engine(const std::string title, const Vec2d screenSize, const Vec2d pixelSize, const int frameRate);
	~Engine();

	void start();
	void stop() { quit_ = true; }

	Vec2d getWindowSize() const;
	void setWindowSize(Vec2d windowSize);
	Vec2d getScreenSize() const;
	void setScreenSize(Vec2d screenSize);
	Vec2d getRenderScale() const;
	void setRenderScale(Vec2d pixelSize);
	void resizeWindow(Vec2d pos, Vec2d size);

	void clearWindow() const;
	void drawPoint(const Vec2d pos, const ColorRGBA color) const;
	void drawLine(const Vec2d start, const Vec2d end, const ColorRGBA color) const;
	void fillRectangle(const Vec2d pos, const int width, const int height, const ColorRGBA color) const;
	void drawRectangle(const Vec2d pos, const int width, const int height, const ColorRGBA color) const;
	void fillCircle(const Vec2d pos, const int radius, const ColorRGBA color) const;
	void drawCircle(const Vec2d pos, const int radius, const ColorRGBA color) const;
	void drawTexture(const std::shared_ptr<SDL_Texture> texture) const;
	void drawTexture(const std::shared_ptr<SDL_Texture> texture, const SDL_Rect dst) const;
	void drawTexture(const std::shared_ptr<SDL_Texture> texture, const SDL_Rect src, const SDL_Rect dst) const;
	void drawSpriteFromSheet(const SDL_Rect src, const SDL_Rect dst, SDL_Texture *spritesheet) const;
	void drawSpriteFromSheet(const SDL_Rect src, const SDL_FRect dst, SDL_Texture *spritesheet) const;
	void drawSpriteFromSheet(const SDL_Rect src, const SDL_Rect dst, SDL_Texture *spritesheet, double angle,
	                                 SDL_Point *center, SDL_RendererFlip flip) const;
	SDL_Texture *loadTexture(const std::string path) const;
	void drawText(SDL_Rect dst, const std::string text) const;

	// Get the state of all keys
	const std::array<KeyState, SDL_NUM_SCANCODES> &getKeyStates() const { return keyboard_.getKeyStates(); }
	// Get the state of a specific key
	const KeyState getKeyState(const SDL_Scancode key) const { return keyboard_.getKeyState(key); }
	// Returns the state of a specific mouse button.
	const KeyState &getMouseButtonState(const Uint8 button) const { return mouse_.getButtonState(button); }
	// Returns the current mouse position.
	const Vec2d &getMousePosition() const { return mouse_.getPosition(); }
	// Returns the mouse wheel delta since the last frame.
	const Vec2d &getMouseWheelDelta() const { return mouse_.getWheelDelta(); }

	double getFPS() const { return fpsCounter_.getFPS(); }

	virtual bool onStart() = 0;
	virtual bool onUpdate(double deltaTime) = 0;
	virtual bool onDestroy() = 0;

	SDL_Rect viewport = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

  private:
	std::unique_ptr<SDL_Window, SDL_Deleter> window_;
	std::unique_ptr<SDL_Renderer, SDL_Deleter> renderer_;
	std::unique_ptr<TTF_Font, SDL_Deleter> font_;

	const std::string title_;
	Vec2d screenSize_;
	Vec2d windowSize_;
	Vec2d pixelSize_;

	bool quit_;

	Keyboard keyboard_;
	Mouse mouse_;
	FPSCounter fpsCounter_;
	FrameRateLimiter frameRateLimiter_;
};
