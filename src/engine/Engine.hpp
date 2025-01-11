#pragma once

#define SDL_MAIN_HANDLED

#include "../constants.hpp"
#include "ColorRGBA.hpp"
#include "FPSCounter.hpp"
#include "FrameRateLimiter.hpp"
#include "FrameTimer.hpp"
#include "KeyState.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Rectf.hpp"
#include "Recti.hpp"
#include "SDL_Deleter.hpp"
#include "Vec2f.hpp"
#include "Vec2i.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

class Engine {
  public:
	Engine(const std::string title, const Vec2i screenSize, const Vec2i pixelSize, const int frameRate);
	~Engine();

	void start();
	void stop() { quit_ = true; }

	Vec2i getWindowSize() const;
	void setWindowSize(Vec2i windowSize);
	Vec2i getScreenSize() const;
	void setScreenSize(Vec2i screenSize);
	Vec2i getRenderScale() const;
	void setRenderScale(Vec2i pixelSize);
	void resizeWindow(Vec2i pos, Vec2i size);

	void clearWindow() const;
	void drawPoint(const Vec2i &pos, const ColorRGBA &color) const;
	void drawPoint(const Vec2f &pos, const ColorRGBA &color) const;
	void drawLine(const Vec2i &start, const Vec2i &end, const ColorRGBA &color) const;
	void drawLine(const Vec2f &start, const Vec2f &end, const ColorRGBA &color) const;
	void fillRectangle(const Vec2i &pos, const int &width, const int &height, const ColorRGBA &color) const;
	void fillRectangle(const Vec2f &pos, const int &width, const int &height, const ColorRGBA &color) const;
	void drawRectangle(const Vec2i &pos, const int &width, const int &height, const ColorRGBA &color) const;
	void drawRectangle(const Vec2f &pos, const int &width, const int &height, const ColorRGBA &color) const;
	void fillCircle(const Vec2i &pos, const int &radius, const ColorRGBA &color) const;
	void fillCircle(const Vec2f &pos, const int &radius, const ColorRGBA &color) const;
	void drawCircle(const Vec2i &pos, const int &radius, const ColorRGBA &color) const;
	void drawCircle(const Vec2f &pos, const int &radius, const ColorRGBA &color) const;
	void drawTexture(const std::shared_ptr<SDL_Texture> texture) const;
	void drawTexture(const std::shared_ptr<SDL_Texture> texture, const SDL_Rect dst) const;
	void drawTexture(const std::shared_ptr<SDL_Texture> texture, const SDL_Rect src, const SDL_Rect dst) const;
	void drawSpriteFromSheet(const Recti &src, const Recti &dst, SDL_Texture *spritesheet) const;
	void drawSpriteFromSheet(const Recti &src, const Rectf &dst, SDL_Texture *spritesheet) const;
	void drawSpriteFromSheet(const Recti &src, const Recti &dst, SDL_Texture *spritesheet, double angle,
	                         SDL_Point *center, SDL_RendererFlip flip) const;
	SDL_Texture *loadTexture(const std::string &path) const;
	void drawText(const Recti &dst, const std::string &text) const;

	// Get the state of all keys
	const std::array<KeyState, SDL_NUM_SCANCODES> &getKeyStates() const { return keyboard_.getKeyStates(); }
	// Get the state of a specific key
	const KeyState getKeyState(const SDL_Scancode key) const { return keyboard_.getKeyState(key); }
	// Returns the state of a specific mouse button.
	const KeyState &getMouseButtonState(const Uint8 button) const { return mouse_.getButtonState(button); }
	// Returns the current mouse position.
	const Vec2i &getMousePosition() const { return mouse_.getPosition(); }
	// Returns the mouse wheel delta since the last frame.
	const Vec2i &getMouseWheelDelta() const { return mouse_.getWheelDelta(); }

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
	Vec2i screenSize_;
	Vec2i windowSize_;
	Vec2i pixelSize_;

	bool quit_;

	Keyboard keyboard_;
	Mouse mouse_;
	FPSCounter fpsCounter_;
	FrameRateLimiter frameRateLimiter_;
};
