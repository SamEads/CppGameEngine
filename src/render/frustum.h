#pragma once

#include <array>
#include "../utility/vec.h"
#include "../physics/aabb.h"

class Frustum
{
private:
	enum class Planes
	{
        BACK = 0,
        FRONT,
        BOTTOM,
        TOP,
        RIGHT,
        LEFT,
        COUNT
	};
	std::array<Vec4<double>, static_cast<size_t>(Planes::COUNT)> m_planes { 0.0 };

public:
    Frustum(double x, double y, double z);
    Frustum() = default;
    void extract(double x, double y, double z);
    bool aabbIn(const AABB& bb) const;
};