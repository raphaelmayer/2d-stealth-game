#pragma once

#include "../types/Vec2i.hpp"
#include "KeyState.hpp"
#include <SDL.h>
#include <array>

#define NUM_MOUSE_BUTTONS 5

class Mouse {
  public:
	Mouse() {}

	// Returns the state of a specific mouse button.
	// Mouse buttons are 0-based (0 = Left, 1 = Middle, etc.)
	const KeyState &getButtonState(const Uint8 button) const { return buttonStates_[button]; }
	// Returns the current state of all buttons.
	// Mouse buttons are 0-based (0 = Left, 1 = Middle, etc.)
	const std::array<KeyState, NUM_MOUSE_BUTTONS> &getButtonStates() const { return buttonStates_; }

	// Returns the current mouse position.
	const Vec2i &getPosition() const { return position_; }

	// Returns the mouse wheel delta since the last frame.
	const Vec2i &getWheelDelta() const { return wheelDelta_; }

	// Updates the mouse state based on the SDL event.
	// SDL mouse buttons are 1-based (1 = Left, 2 = Middle, etc.)
	void update(const SDL_Event &event)
	{
		int buttonIndex;
		switch (event.type) {
		case SDL_MOUSEBUTTONDOWN:
			buttonIndex = static_cast<int>(event.button.button - 1);
			buttonStates_[buttonIndex].pressed = true;
			buttonStates_[buttonIndex].held = true;
			break;

		case SDL_MOUSEBUTTONUP:
			buttonIndex = static_cast<int>(event.button.button - 1);
			buttonStates_[buttonIndex].held = false;
			buttonStates_[buttonIndex].released = true;
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
	std::array<KeyState, NUM_MOUSE_BUTTONS> buttonStates_{}; // Supports up to NUM_MOUSE_BUTTONS mouse buttons.
	Vec2i position_{0, 0};                                   // Current mouse position.
	Vec2i wheelDelta_{0, 0};                                 // Mouse wheel delta since last frame.
};
