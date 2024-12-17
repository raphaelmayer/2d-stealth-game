#pragma once

// The engine keeps track of input states. It makes a distinction between:
// - Pressed: The input was pressed this frame.
// - Held: The input is being held down.
// - Released: The input was released this frame.

struct KeyState {
	bool pressed = false;  // True if the key or button was pressed this frame.
	bool held = false;     // True if the key or button is currently held down.
	bool released = false; // True if the key or button was released this frame.
};
