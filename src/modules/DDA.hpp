#pragma once

#include <vector>
#include "../engine/Vec2i.hpp"
#include "../engine/Vec2f.hpp"
#include <iostream>
#include <cmath> // for sqrt

class DDA {
  public:
	static bool castRay(const std::vector<std::vector<int>> &map, Vec2i startTile, Vec2i endTile)
	{
		// Offset the start and end to the center of tiles
		Vec2f start = Vec2f(startTile.x + 0.5f, startTile.y + 0.5f);
		Vec2f end = Vec2f(endTile.x + 0.5f, endTile.y + 0.5f);

		// Calculate direction
		Vec2f dir = end - start;
		float magnitude = std::sqrt(dir.x * dir.x + dir.y * dir.y);
		if (magnitude == 0.0f) {
			return false; // No direction, invalid ray
		}
		dir = dir / magnitude;

		// Determine the step size and initial ray lengths
		Vec2f unitStepSize = {std::sqrt(1.0f + (dir.y * dir.y) / (dir.x * dir.x)),
		                     std::sqrt(1.0f + (dir.x * dir.x) / (dir.y * dir.y))};

		Vec2i mapCoords = {(int)std::floor(start.x), (int)std::floor(start.y)};
		Vec2i step = {(dir.x < 0) ? -1 : 1, (dir.y < 0) ? -1 : 1};

		// Calculate initial rayLength.x, rayLength.y to the first boundary
		Vec2f rayLength;
		if (dir.x < 0) {
			rayLength.x = (start.x - float(mapCoords.x)) * unitStepSize.x;
		} else {
			rayLength.x = (float(mapCoords.x + 1) - start.x) * unitStepSize.x;
		}

		if (dir.y < 0) {
			rayLength.y = (start.y - float(mapCoords.y)) * unitStepSize.y;
		} else {
			rayLength.y = (float(mapCoords.y + 1) - start.y) * unitStepSize.y;
		}

		bool tileFound = false;
		float distance = 0.0f;
		float maxDistance = 100.0f; // Adjust as needed

		std::vector<Vec2f> path;
		path.push_back(Vec2f((float)mapCoords.x, (float)mapCoords.y));

		while (!tileFound && distance < maxDistance) {
			// Step along the ray
			if (rayLength.x < rayLength.y) {
				mapCoords.x += step.x;
				distance = rayLength.x;
				rayLength.x += unitStepSize.x;
			} else {
				mapCoords.y += step.y;
				distance = rayLength.y;
				rayLength.y += unitStepSize.y;
			}

			path.push_back(Vec2f((float)mapCoords.x, (float)mapCoords.y));

			// Check bounds before indexing
			if (mapCoords.x >= 0 && mapCoords.x < (int)map[0].size() && mapCoords.y >= 0 && mapCoords.y < (int)map.size()) {
				if (map[mapCoords.y][mapCoords.x] == 1) { // collision check
					tileFound = true;
				}
			}

			// Stop, if we reached the end tile.
			if (mapCoords.x == (int)std::floor(end.x) && mapCoords.y == (int)std::floor(end.y)) 
				break;
		}

		// std::cout << "Path: ";
		// for (auto &p : path)
		// 	std::cout << "(" << p.x << "," << p.y << "), ";
		// std::cout << "tileFound: " << tileFound << " target:(" << end.x << "," << end.y << ")" << std::endl;

		// If needed, we can compute the exact intersection point:
		// Vec2f intersection = start + dir * distance;

		return tileFound;
	}
};
