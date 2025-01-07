#pragma once

#include "../constants.hpp"
#include <cassert>
#include <cmath>
//#include "Vf2d.hpp"

struct Vec2d {
	int x;
	int y;

	bool operator==(const Vec2d &v) const { return x == v.x && y == v.y; }
	bool operator!=(const Vec2d &v) const { return !operator==(v); }

	Vec2d operator+(int n) const { return {x + n, y + n}; }
	Vec2d operator+(const Vec2d &v) const { return {x + v.x, y + v.y}; }

	Vec2d operator-(int n) const { return {x - n, y - n}; }
	Vec2d operator-(const Vec2d &v) const { return {x - v.x, y - v.y}; }

	Vec2d operator*(const Vec2d &v) const { return {x * v.x, y * v.y}; }
	Vec2d operator*(int n) const { return {x * n, y * n}; }
	Vec2d operator*(float n) const { return {(int)((float)x * n), (int)((float)y * n)}; }

	Vec2d operator/(const Vec2d &v) const
	{
		assert(v.x != 0 && v.y != 0);
		return {x / v.x, y / v.y};
	}
	Vec2d operator/(int n) const
	{
		assert(n != 0);
		return {x / n, y / n};
	}
	Vec2d operator/(float n) const
	{
		assert(n != 0);
		return {(int)((float)x / n), (int)((float)y / n)};
	}

	Vec2d &operator+=(const Vec2d &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vec2d &operator+=(int n)
	{
		x += n;
		y += n;
		return *this;
	}

	Vec2d &operator-=(const Vec2d &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vec2d &operator-=(int n)
	{
		x -= n;
		y -= n;
		return *this;
	}

	Vec2d &operator*=(const Vec2d &v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vec2d &operator*=(int n)
	{
		x *= n;
		y *= n;
		return *this;
	}

	Vec2d &operator/=(const Vec2d &v)
	{
		assert(v.x != 0 && v.y != 0);
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vec2d &operator/=(int n)
	{
		assert(n != 0);
		x /= n;
		y /= n;
		return *this;
	}

	// Convert to 1D (assuming a certain width for the 2D space)
	int to1d(int width) const { return y * width + x; }

	// Sign function
	Vec2d sign() const { return {(x > 0) - (x < 0), (y > 0) - (y < 0)}; }

	// Convert to Vf2d (float version of the vector)
	//Vf2d toVf2d() const { return Vf2d{static_cast<float>(x), static_cast<float>(y)}; }

	// convert vector from pixel domain to tile domain. e.g.: (32,32).toTileSize() == (2,2)
	// also rounds the result.
	Vec2d toTileSize() const { return operator/(TILE_SIZE); }

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(x, y);
	}
};