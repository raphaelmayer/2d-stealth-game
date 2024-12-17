#pragma once

#include "KeyState.hpp"
#include "Vec2d.hpp"
#include <SDL.h>
#include <array>

class Mouse {
  public:
	Mouse() {}

	// Returns the state of a specific mouse button.
	const KeyState &getButtonState(const Uint8 button) const
	{
		return buttonStates_[button - 1]; // SDL mouse buttons are 1-based (1 = Left, 2 = Middle, etc.)
	}

	// Returns the current mouse position.
	const Vec2d &getPosition() const { return position_; }

	// Returns the mouse wheel delta since the last frame.
	const Vec2d &getWheelDelta() const { return wheelDelta_; }

	// Updates the mouse state based on the SDL event.
	void update(const SDL_Event &event)
	{
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			buttonStates_[event.button.button - 1].pressed = true;
			buttonStates_[event.button.button - 1].held = true;
			break;

		case SDL_MOUSEBUTTONUP:
			buttonStates_[event.button.button - 1].held = false;
			buttonStates_[event.button.button - 1].released = true;
			break;

		case SDL_MOUSEMOTION:
			position_.x = event.motion.x;
			position_.y = event.motion.y;
			break;

		case SDL_MOUSEWHEEL:
			wheelDelta_.x = event.wheel.x;
			wheelDelta_.y = event.wheel.y;
			break;

		default:
			break;
		}
	}

	// Resets transient states (`pressed`, `released`, and `wheelDelta`) at the end of each frame.
	void reset()
	{
		for (auto &state : buttonStates_) {
			state.pressed = false;
			state.released = false;
		}
		wheelDelta_ = {0, 0};
	}

  private:
	std::array<KeyState, 5> buttonStates_{}; // Supports up to 5 mouse buttons.
	Vec2d position_{0, 0};               // Current mouse position.
	Vec2d wheelDelta_{0, 0};             // Mouse wheel delta since last frame.
};
