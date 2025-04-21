#pragma once

#include<chrono>

class FrameTimer {
  public:
	FrameTimer() : lastFrameTime(clock::now()) {}

	// Called at the beginning of each frame
	void update()
	{
		auto currentFrameTime = clock::now();
		deltaTime = std::chrono::duration<double, period>(currentFrameTime - lastFrameTime).count();
		lastFrameTime = currentFrameTime;
	}

	// Get the delta time in seconds
	double getDeltaTime() const { return deltaTime; }

  private:
	using clock = std::chrono::high_resolution_clock;
	using timePoint = std::chrono::time_point<clock>;
	using period = std::chrono::seconds::period;

	timePoint lastFrameTime;
	double deltaTime = 0.0f;
};

// i thought about making getDeltaTime more descriptive by using a type like seconds (as suggested in one of
// the lectures), but 
//		1. I did not want to leak implementation details or 
//		2. deal with having to cast deltaTime everywhere. 
// 
// I also thought about nameing it getDeltaTimeInSeconds, but I don't like the verbosity. This is not JAVA lol. 
// And since all respectable IDE's provide comments on mouseover etc, I felt that this solution 
// is probably good enough while not being a huge pain to use. 