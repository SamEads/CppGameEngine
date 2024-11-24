#pragma once

namespace MathTools
{
    template<class T>
    const T& clamp(const T& v, const T& lo, const T& hi)
    {
        if (v < lo)
            return lo;
        if (hi < v)
            return hi;
        return v;
    }
}