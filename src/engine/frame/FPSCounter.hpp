#pragma once

#include <chrono>
#include <iostream>

class FPSCounter {
  public:
	FPSCounter() : frameCount(0), updateInterval(std::chrono::milliseconds(1000)), fps(0)
	{
		lastUpdateTime = std::chrono::steady_clock::now();
	}

	void update()
	{
		frameCount++;
		auto now = std::chrono::steady_clock::now();
		auto elapsedSinceUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime);

		if (elapsedSinceUpdate >= updateInterval) {
			fps = static_cast<unsigned>(frameCount * 1000.0 / elapsedSinceUpdate.count());
			frameCount = 0;
			lastUpdateTime = now;
		}
	}

	unsigned getFPS() const { return fps; }

	void printFPS() const { std::cout << "FPS: " << fps << std::endl; };

  private:
	unsigned frameCount;
	std::chrono::steady_clock::time_point lastUpdateTime;
	std::chrono::milliseconds updateInterval;
	unsigned fps;
};
