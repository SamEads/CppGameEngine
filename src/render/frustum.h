#pragma once

#include <map>
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
	std::map<Planes, Vec4<double>> m_planes;

public:
    Frustum(double x, double y, double z);
    Frustum() = default;
    void extract(double x, double y, double z);
    bool aabbIn(const AABB& bb) const;
};