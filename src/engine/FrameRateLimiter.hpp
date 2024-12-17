#pragma once

#include<chrono>
#include <thread>

class FrameRateLimiter {
  public:
	explicit FrameRateLimiter(unsigned int targetFPS) { frameDuration = std::chrono::milliseconds(1000 / targetFPS); }
	
	void startFrame() { frameStartTime = std::chrono::steady_clock::now(); }
	
	void endFrame() const
	{
		auto now = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - frameStartTime);
		auto sleepDuration = frameDuration - elapsedTime;

		if (sleepDuration.count() > 0)
			std::this_thread::sleep_for(sleepDuration);
	}

  private:
	std::chrono::milliseconds frameDuration;
	std::chrono::steady_clock::time_point frameStartTime;
};
