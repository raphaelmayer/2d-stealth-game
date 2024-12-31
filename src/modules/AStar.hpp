#pragma once

#include "../engine/Vec2d.hpp"
#include <algorithm>
#include <climits>
#include <cmath>
#include <queue>
#include <vector>

struct Node {
	Node(Vec2d pos, Node *parent_) : position(pos), parent(parent_), g(INT_MAX), h(INT_MAX) {}

	int getScore() const { return g + h; }

	Vec2d position;
	Node *parent;
	int g;
	int h;
};

auto pQueueCmp = [](Node *a, Node *b) { return a->getScore() > b->getScore(); };

class AStar {
  public:
	// Finds the shortest path between two points on a 2D grid using the A* algorithm.
	// The map is a grid of booleans, where 1 indicates a blocking tile.
	// This implementation expects start and target to be in tile space.
	static std::vector<Vec2d> findPath(const std::vector<std::vector<int>> &map, Vec2d start, Vec2d target)
	{
		// Check if start and target positions are within bounds
		if (!isInBounds(map, start) || !isInBounds(map, target)) {
			return {}; // Return empty path if out of bounds
		}

		// Check if start and target positions are blocked
		if (map[start.y][start.x] != 0 || map[target.y][target.x] != 0) {
			return {}; // Return empty path if start or target is blocked
		}

		// Priority queue with a custom comparator
		std::priority_queue<Node *, std::vector<Node *>, decltype(pQueueCmp)> queue(pQueueCmp);

		std::vector<Node *> visited;

		// Start node
		Node *startNode = new Node(start, nullptr);
		startNode->g = 0;
		startNode->h = heuristic(start, target);
		queue.push(startNode);

		while (!queue.empty()) {
			Node *currentNode = queue.top();
			queue.pop();

			// Check if target is reached
			if (currentNode->position == target) {
				std::vector<Vec2d> path = reconstructPath(currentNode);
				cleanUpMemory(queue, visited);
				return path;
			}

			// Skip if already visited
			if (isVisited(visited, currentNode->position)) {
				delete currentNode;
				continue;
			}

			visited.push_back(currentNode);

			// Directions for neighbors
			// static const Vec2d directions[8] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1,
			// -1}};
			static const Vec2d directions[4] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};

			// Check neighbors
			for (int i = 0; i < 4; ++i) {
				Vec2d nextPosition = currentNode->position + directions[i];

				// Check bounds
				if (!isInBounds(map, nextPosition))
					continue;

				// Check collision
				if (map[nextPosition.y][nextPosition.x] != 0)
					continue;

				int cost = currentNode->g + ((i & 1) ? 14 : 10); // Diagonal: 14, Straight: 10

				// Check if already visited
				if (!isVisited(visited, nextPosition)) {
					Node *newNode = new Node(nextPosition, currentNode);
					newNode->g = cost;
					newNode->h = heuristic(nextPosition, target);
					queue.push(newNode);
				}
			}
		}

		cleanUpMemory(queue, visited);
		return {}; // Return empty path if no path found
	}

  private:
	static int heuristic(Vec2d start, Vec2d end)
	{
		// Euclidean distance multiplied by 10 (scaled for integer arithmetic)
		Vec2d delta = end - start;
		return static_cast<int>(10 * std::sqrt(delta.x * delta.x + delta.y * delta.y));
	}

	static bool isInBounds(const std::vector<std::vector<int>> &map, Vec2d pos)
	{
		return pos.x >= 0 && pos.y >= 0 && pos.y < map.size() && pos.x < map[0].size();
	}

	static bool isVisited(const std::vector<Node *> &visited, Vec2d position)
	{
		return std::any_of(visited.begin(), visited.end(),
		                   [&position](Node *node) { return node->position == position; });
	}

	static std::vector<Vec2d> reconstructPath(Node *node)
	{
		std::vector<Vec2d> path;
		while (node) {
			path.push_back(node->position);
			node = node->parent;
		}
		std::reverse(path.begin(), path.end());
		return path;
	}

	static void cleanUpMemory(std::priority_queue<Node *, std::vector<Node *>, decltype(pQueueCmp)> &queue,
	                          std::vector<Node *> &visited)
	{
		while (!queue.empty()) {
			delete queue.top();
			queue.pop();
		}
		for (Node *node : visited) {
			delete node;
		}
	}
};
