#pragma once

#include "../../constants.hpp"
#include <cassert>
#include <cmath>

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

	float length() const { return std::sqrt(x * x + y * y); }

	template <class Archive>
	void serialize(Archive &archive)
	{
		archive(x, y);
	}
};