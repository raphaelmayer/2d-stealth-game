#include "Input.hpp"
#include "../Engine.hpp"

Input::Input(const Engine &engine) : engine_(engine)
{
}

void Input::update(const SDL_Event &event)
{
	// It would be neat if we could reset here directly, but this hinges on how we consume the sdl events each frame.

	keyboard_.update(event);
	mouse_.update(event);
}

void Input::reset()
{
	keyboard_.reset();
	mouse_.reset();
}

const std::array<KeyState, SDL_NUM_SCANCODES> &Input::getKeyStates() const
{
	return keyboard_.getKeyStates();
}

const KeyState &Input::getKeyState(SDL_Scancode key) const
{
	return keyboard_.getKeyState(key);
}

const std::array<KeyState, NUM_MOUSE_BUTTONS> &Input::getMouseButtonStates() const
{
	return mouse_.getButtonStates();
}

const KeyState &Input::getMouseButtonState(Uint8 button) const
{
	return mouse_.getButtonState(button);
}

Vec2i Input::getMousePosition() const
{
	return mouse_.getPosition() / engine_.getRenderScale();
}

const Vec2i &Input::getMouseWheelDelta() const
{
	return mouse_.getWheelDelta();
}