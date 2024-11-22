#pragma once

#include <iostream>

template <typename T>
struct Vec2
{
    T x = 0, y = 0;

    // Assignment operator
    template <typename T>
    Vec2& operator=(const Vec2<T>& other)
    {
        x = other.x;
        y = other.y;
        return *this;
    }

    // Prefix
    Vec2& operator++()
    {
        x++; y++;
        return *this;
    }

    // Postfix
    Vec2 operator++(int)
    {
        Vec2 old = *this;
        x++; y++;
        return old;
    }

    template <typename U>
    bool operator==(const Vec2<U>& other) const
    {
        return x == other.x && y == other.y;
    }

    template <typename U>
    bool operator!=(const Vec2<U>& other) const
    {
        return !(*this == other);
    }

    template <typename U>
    Vec2& operator+=(const Vec2<U>& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    template <typename U>
    Vec2& operator-=(const Vec2<U>& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(const T& scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vec2& operator/=(const T& scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template <typename U>
    Vec2 operator+(const Vec2<U>& other) const
    {
        return { x + static_cast<T>(other.x), y + static_cast<T>(other.y) };
    }

    template <typename U>
    Vec2 operator-(const Vec2<U>& other) const
    {
        return { x - static_cast<T>(other.x), y - static_cast<T>(other.y) };
    }

    Vec2 operator*(const T& scalar) const
    {
        return { x * scalar, y * scalar };
    }

    Vec2 operator/(const T& scalar) const
    {
        return { x / scalar, y / scalar };
    }
};

template <typename T>
struct Vec3
{
    T x = 0, y = 0, z = 0;

    // Assignment operator
    template <typename U>
    Vec3<T>& operator=(const Vec3<U>& other)
    {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        z = static_cast<T>(other.z);
        return *this;
    }

    // Prefix
    Vec3& operator++()
    {
        x++; y++; z++;
        return *this;
    }

    // Postfix
    Vec3 operator++(int)
    {
        Vec3 old = *this;
        x++; y++; z++;
        return old;
    }

    Vec3& operator+=(const Vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3& operator-=(const Vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3& operator*=(const T& scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3& operator/=(const T& scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    Vec3 operator+(const Vec3& other) const
    {
        return { x + other.x, y + other.y, z + other.z };
    }

    Vec3 operator-(const Vec3& other) const
    {
        return { x - other.x, y - other.y, z - other.z };
    }

    Vec3 operator*(const T& scalar) const
    {
        return { x * scalar, y * scalar, z * scalar };
    }

    Vec3 operator/(const T& scalar) const
    {
        return { x / scalar, y / scalar, z / scalar };
    }

    Vec3 normalize()
    {
        Vec3 result = *this;

        float length = sqrtf(x * x + y * y + z * z);
        if (length != 0.0f)
        {
            float ilen = 1.0f / length;

            result.x *= ilen;
            result.y *= ilen;
            result.z *= ilen;
        }

        return result;
    }

    template <typename U>
    Vec3 cross(Vec3<U> other)
    {
        return {
            y * static_cast<T>(other.z) - z * static_cast<T>(other.y),
            z * static_cast<T>(other.x) - x * static_cast<T>(other.z),
            x * static_cast<T>(other.y) - y * static_cast<T>(other.x)
        };
    }
};

template <typename T>
struct Vec4
{
    T x, y, z, w;
};

template <typename T>
struct Vec2r : public Vec2<T>
{
    Vec2<T> prev;
    
    void update()
    {
        prev = *this;
    }

    // Assignment operator
    template <typename U>
    Vec2r& operator=(const Vec2<U>& other)
    {
        this->x = static_cast<T>(other.x);
        this->y = static_cast<T>(other.y);
        return *this;
    }

    Vec2<T> interpolated(const float alpha) const
    {
        return interpolate(prev, *this, alpha);
    }
};

template <typename T>
struct Vec3r : public Vec3<T>
{
private:
    Vec3<T> m_prev { 0 };

public:
    void update()
    {
        m_prev = *this;
    }

    // Assignment operator
    template <typename U>
    Vec3r& operator=(const Vec3<U>& other)
    {
        this->x = static_cast<T>(other.x);
        this->y = static_cast<T>(other.y);
        this->z = static_cast<T>(other.z);
        return *this;
    }

    Vec3<T> interpolated(const float alpha) const
    {
        return interpolate(m_prev, *this, alpha);
    }
};