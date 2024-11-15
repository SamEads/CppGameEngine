#pragma once

template <typename T>
inline T interpolate(T start, T end, const float alpha)
{
	return start + (end - start) * alpha;
}

template <typename T>
inline Vec3<T> interpolate(Vec3<T> start, Vec3<T> end, const float alpha)
{
	return {
		start.x + (end.x - start.x) * alpha,
		start.y + (end.y - start.y) * alpha,
		start.z + (end.z - start.z) * alpha
	};
}

template <typename T>
inline Vec2<T> interpolate(Vec2<T> start, Vec2<T> end, const float alpha)
{
	return {
		start.x + (end.x - start.x) * alpha,
		start.y + (end.y - start.y) * alpha,
	};
}