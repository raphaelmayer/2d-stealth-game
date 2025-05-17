#pragma once

#define SDL_MAIN_HANDLED

#include "../constants.hpp"
#include "SDL_Deleter.hpp"
#include "audio/Audio.hpp"
#include "frame/FPSCounter.hpp"
#include "frame/FrameRateLimiter.hpp"
#include "frame/FrameTimer.hpp"
#include "input/Input.hpp"
#include "input/Keyboard.hpp"
#include "input/Mouse.hpp"
#include "types.hpp"
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
	void stop()
	{
		quit_ = true;
	}

	Vec2i getWindowSize() const;
	void setWindowSize(Vec2i windowSize);
	void setWindowFullscreen();
	void setWindowWindowed(); // TODO -> IMPLEMENT
	Vec2i getScreenSize() const;
	void setScreenSize(Vec2i screenSize);
	int getRenderScale() const;
	void setRenderScale(Vec2i pixelSize); // TODO: this render scale should be a scalar.
	void resizeWindow(Vec2i pos, Vec2i size);

	void clearWindow() const;
	void drawPoint(const Vec2i &pos, const ColorRGBA &color) const;
	void drawPoint(const Vec2f &pos, const ColorRGBA &color) const;
	void drawLine(const Vec2i &start, const Vec2i &end, const ColorRGBA &color) const;
	void drawLine(const Vec2f &start, const Vec2f &end, const ColorRGBA &color) const;
	void fillRectangle(const Recti &rect, const ColorRGBA &color) const;
	void fillRectangle(const Rectf &rect, const ColorRGBA &color) const;
	void drawRectangle(const Recti &rect, const ColorRGBA &color) const;
	void drawRectangle(const Rectf &rect, const ColorRGBA &color) const;
	void fillCircle(const Vec2i &pos, const int &radius, const ColorRGBA &color) const;
	void fillCircle(const Vec2f &pos, const int &radius, const ColorRGBA &color) const;
	void drawCircle(const Vec2i &pos, const int &radius, const ColorRGBA &color) const;
	void drawCircle(const Vec2f &pos, const int &radius, const ColorRGBA &color) const;
	void drawTexture(const Texture &texture) const;
	void drawTexture(const Texture &texture, const Recti &dst) const;
	void drawTexture(const Texture &texture, const Rectf &dst) const;
	void drawTexture(const Texture &texture, const Recti &src, const Recti &dst) const;
	void drawTexture(const Texture &texture, const Recti &src, const Rectf &dst) const;
	void drawTexture(const Texture &texture, const Recti &src, const Recti &dst, const double &angle,
	                 const Vec2i &center, const TextureFlip &flip) const;
	void drawTexture(const Texture &texture, const Recti &src, const Rectf &dst, const double &angle,
	                 const Vec2f &center, const TextureFlip &flip) const;
	SDL_Texture *loadSDLTexture(const std::string &path) const;
	Texture loadTexture(const std::string &path) const;
	void drawText(const Recti &dst, const std::string &text) const;
	void drawText(const Rectf &dst, const std::string &text) const;

	void enableAlphaBlending();
	void disableAlphaBlending();

	const Input &input() const
	{
		return input_;
	}

	Input &input()
	{
		return input_;
	}

	double getFPS() const
	{
		return fpsCounter_.getFPS();
	}

	Audio &getAudio()
	{
		return audio_;
	}

	const Audio &getAudio() const
	{
		return audio_;
	}

	virtual bool onStart() = 0;
	virtual bool onUpdate(double deltaTime) = 0;
	virtual bool onDestroy() = 0;

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

	Audio audio_;
	Input input_;
};
