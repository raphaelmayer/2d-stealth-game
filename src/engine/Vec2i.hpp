#pragma once

#include "../constants.hpp"
#include <cassert>
#include <cmath>
//#include "Vec2f.hpp"

struct Vec2i {
	int x;
	int y;

	bool operator==(const Vec2i &v) const { return x == v.x && y == v.y; }
	bool operator!=(const Vec2i &v) const { return !operator==(v); }

	Vec2i operator+(int n) const { return {x + n, y + n}; }
	Vec2i operator+(const Vec2i &v) const { return {x + v.x, y + v.y}; }

	Vec2i operator-(int n) const { return {x - n, y - n}; }
	Vec2i operator-(const Vec2i &v) const { return {x - v.x, y - v.y}; }

	Vec2i operator*(const Vec2i &v) const { return {x * v.x, y * v.y}; }
	Vec2i operator*(int n) const { return {x * n, y * n}; }
	Vec2i operator*(float n) const { return {(int)std::round((float)x * n), (int)std::round((float)y * n)}; }

	Vec2i operator/(const Vec2i &v) const
	{
		assert(v.x != 0 && v.y != 0);
		return {x / v.x, y / v.y};
	}
	Vec2i operator/(int n) const
	{
		assert(n != 0);
		return {x / n, y / n};
	}
	Vec2i operator/(float n) const
	{
		assert(n != 0);
		return {(int)std::round((float)x / n), (int)std::round((float)y / n)};
	}

	Vec2i &operator+=(const Vec2i &v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	Vec2i &operator+=(int n)
	{
		x += n;
		y += n;
		return *this;
	}

	Vec2i &operator-=(const Vec2i &v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	Vec2i &operator-=(int n)
	{
		x -= n;
		y -= n;
		return *this;
	}

	Vec2i &operator*=(const Vec2i &v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	Vec2i &operator*=(int n)
	{
		x *= n;
		y *= n;
		return *this;
	}

	Vec2i &operator/=(const Vec2i &v)
	{
		assert(v.x != 0 && v.y != 0);
		x /= v.x;
		y /= v.y;
		return *this;
	}

	Vec2i &operator/=(int n)
	{
		assert(n != 0);
		x /= n;
		y /= n;
		return *this;
	}

	// Convert to 1D (assuming a certain width for the 2D space)
	int to1d(int width) const { return y * width + x; }

	// Sign function
	Vec2i sign() const { return {(x > 0) - (x < 0), (y > 0) - (y < 0)}; }

	// Convert to Vec2f (float version of the vector)
	//Vec2f toVf2d() const { return Vec2f{static_cast<float>(x), static_cast<float>(y)}; }

	// convert vector from pixel domain to tile domain. e.g.: (32,32).toTileSize() == (2,2)
	// also rounds the result.
	Vec2i toTileSize() const { return operator/(TILE_SIZE); }

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(x, y);
	}
};