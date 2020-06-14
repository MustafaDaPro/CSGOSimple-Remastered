#pragma once

template <class T>
inline void NormalizeAngles(T& vec)
{

	for (auto i = 0; i < 2; i++)
	{

		while (vec[i] < -180.0f) vec[i] += 360.0f;
		while (vec[i] > 180.0f) vec[i] -= 360.0f;

	}
}