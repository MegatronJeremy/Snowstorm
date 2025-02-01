#pragma once

#include <cmath>

namespace Snowstorm
{
	template <typename T>
	static bool isEqual(const T& a, const T& b)
	{
		return a == b;
	}

	static bool isEqual(const float a, const float b)
	{
		return std::fabs(a - b) < 1e-5f; // Adjust epsilon as needed
	}
}
