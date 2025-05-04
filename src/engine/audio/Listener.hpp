#pragma once

#include "../types.hpp"
#include "Audio.hpp"

//Currently not used as our Listener is always the Camera Position.
class Listener {
  public:
	Listener() : listenerPosition_(Vec2f{0.00f, 0.00f}) {} //using initialization list
	explicit Listener(const Vec2f &desiredPosition) : listenerPosition_(desiredPosition){} //explicit constructor because only 1 parameter

	void update(const Vec2f  &newPosition) { listenerPosition_ = newPosition; } //const infront of type as this is our convention

	const Vec2f& getListenerPosition() const { return listenerPosition_; }

  private:
	Vec2f listenerPosition_;
};