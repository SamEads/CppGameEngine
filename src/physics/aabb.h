#pragma once

struct AABB
{
    double minX, minY, minZ;
    double maxX, maxY, maxZ;

    bool overlaps(const AABB& other) const
    {
        return
            (minX <= other.maxX && maxX >= other.minX) &&
            (minY <= other.maxY && maxY >= other.minY) &&
            (minZ <= other.maxZ && maxZ >= other.minZ);
    }
};