#pragma once

#include "KeyState.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"

class Input {
  public:
	void update(const SDL_Event &event)
	{
		// probably want to reset aswell

		keyboard_.update(event);
		mouse_.update(event);
	}

	void reset()
	{
		keyboard_.reset();
		mouse_.reset();
	}

	// Get the state of all keys
	const std::array<KeyState, SDL_NUM_SCANCODES> &getKeyStates() const
	{
		return keyboard_.getKeyStates();
	}

	// Get the state of a specific key
	const KeyState &getKeyState(const SDL_Scancode key) const
	{
		return keyboard_.getKeyState(key);
	}

	// Returns the current state of all buttons.
	// SDL mouse buttons are 1-based (1 = Left, 2 = Middle, etc.)
	const std::array<KeyState, NUM_MOUSE_BUTTONS> &getMouseButtonStates() const
	{
		return mouse_.getButtonStates();
	}

	// Returns the state of a specific mouse button.
	// SDL mouse buttons are 1-based (1 = Left, 2 = Middle, etc.)
	const KeyState &getMouseButtonState(const Uint8 button) const
	{
		return mouse_.getButtonState(button);
	}

	// Returns the current mouse position.
	const Vec2i getMousePosition() const
	{
		return mouse_.getPosition();
	}

	// Returns the mouse wheel delta since the last frame.
	const Vec2i &getMouseWheelDelta() const
	{
		return mouse_.getWheelDelta();
	}

  private:
	Keyboard keyboard_;
	Mouse mouse_;
};