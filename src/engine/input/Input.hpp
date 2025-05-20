#pragma once

#include "KeyState.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
//#include <SDL.h>

class Engine;

// This class provides a unified interface for all input-related functionality, including keyboard state tracking and
// mouse input handling (position, buttons, wheel).
class Input {
  public:
	// It currently needs access to the engine to scale the mouse position by the rendering scale. This is not ideal but
	// ok for now. We could separate this logic from the input system or pass the scale factor directly and update it
	// via events or a callback. Why the Engine dependency exists:
	// - Mouse coordinates need rendering scale for correct gameplay coordinates
	// - Having Input as a direct Engine component keeps API simple and centralized
	// - Avoids duplicating scale logic across multiple systems
	// - Makes input queries work "out of the box" without manual scaling
	explicit Input(const Engine &engine);

	void update(const SDL_Event &event);
	void reset();

	// Get the state of all keys
	const std::array<KeyState, SDL_NUM_SCANCODES> &getKeyStates() const;

	// Get the state of a specific key
	const KeyState &getKeyState(SDL_Scancode key) const;

	// Returns the current state of all buttons.
	// SDL mouse buttons are 1-based (1 = Left, 2 = Middle, etc.)
	const std::array<KeyState, NUM_MOUSE_BUTTONS> &getMouseButtonStates() const;

	// Returns the state of a specific mouse button.
	// SDL mouse buttons are 1-based (1 = Left, 2 = Middle, etc.)
	const KeyState &getMouseButtonState(Uint8 button) const;

	// Returns the current mouse position.
	Vec2i getMousePosition() const;

	// Returns the mouse wheel delta since the last frame.
	const Vec2i &getMouseWheelDelta() const;

  private:
	const Engine &engine_;

	Keyboard keyboard_;
	Mouse mouse_;
};