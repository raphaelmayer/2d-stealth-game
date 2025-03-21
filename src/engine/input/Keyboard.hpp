#pragma once

#include "KeyState.hpp"
#include <SDL.h>
#include <array>

class Keyboard {
  public:
	Keyboard() {}

	// Returns the current state of all keys.
	const std::array<KeyState, SDL_NUM_SCANCODES> &getKeyStates() const { return keyStates_; }

	// Returns the state of a specific key.
	const KeyState &getKeyState(const SDL_Scancode key) const { return keyStates_[key]; }

	// Updates the key states based on the SDL event.
	void update(const SDL_Event &event)
	{
		if (event.type == SDL_KEYDOWN) {
			// engine keeps track of key presses and makes distinction between pressed and held.
			if (!event.key.repeat) {
				keyStates_[event.key.keysym.scancode].pressed = true;
			}
			keyStates_[event.key.keysym.scancode].held = true;
		} else if (event.type == SDL_KEYUP) {
			keyStates_[event.key.keysym.scancode].held = false;
			keyStates_[event.key.keysym.scancode].released = true;
		}
	}

	void reset()
	{
		for (auto &state : keyStates_) {
			state.pressed = false;
			state.released = false;
		}
	}

  private:
	std::array<KeyState, SDL_NUM_SCANCODES> keyStates_{};
};