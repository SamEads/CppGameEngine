#pragma once

#include <array>

template <typename T>
struct Mat4
{
    T m0, m4, m8, m12;
    T m1, m5, m9, m13;
    T m2, m6, m10, m14;
    T m3, m7, m11, m15;

    Mat4 operator*(const Mat4& right) const
    {
        Mat4 result { 0 };

        result.m0 = m0 * right.m0 + m4 * right.m1 + m8 * right.m2 + m12 * right.m3;
        result.m1 = m1 * right.m0 + m5 * right.m1 + m9 * right.m2 + m13 * right.m3;
        result.m2 = m2 * right.m0 + m6 * right.m1 + m10 * right.m2 + m14 * right.m3;
        result.m3 = m3 * right.m0 + m7 * right.m1 + m11 * right.m2 + m15 * right.m3;

        result.m4 = m0 * right.m4 + m4 * right.m5 + m8 * right.m6 + m12 * right.m7;
        result.m5 = m1 * right.m4 + m5 * right.m5 + m9 * right.m6 + m13 * right.m7;
        result.m6 = m2 * right.m4 + m6 * right.m5 + m10 * right.m6 + m14 * right.m7;
        result.m7 = m3 * right.m4 + m7 * right.m5 + m11 * right.m6 + m15 * right.m7;

        result.m8 = m0 * right.m8 + m4 * right.m9 + m8 * right.m10 + m12 * right.m11;
        result.m9 = m1 * right.m8 + m5 * right.m9 + m9 * right.m10 + m13 * right.m11;
        result.m10 = m2 * right.m8 + m6 * right.m9 + m10 * right.m10 + m14 * right.m11;
        result.m11 = m3 * right.m8 + m7 * right.m9 + m11 * right.m10 + m15 * right.m11;

        result.m12 = m0 * right.m12 + m4 * right.m13 + m8 * right.m14 + m12 * right.m15;
        result.m13 = m1 * right.m12 + m5 * right.m13 + m9 * right.m14 + m13 * right.m15;
        result.m14 = m2 * right.m12 + m6 * right.m13 + m10 * right.m14 + m14 * right.m15;
        result.m15 = m3 * right.m12 + m7 * right.m13 + m11 * right.m14 + m15 * right.m15;

        return result;
    }

	std::array<T, 16> to16()
	{
		return
		{
			m0, m1, m2, m3,
			m4, m5, m6, m7,
			m8, m9, m10, m11,
			m12, m13, m14, m15,
		};
	}

    static Mat4 identity()
    {
        return
        {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }

	static Mat4 lookAt(Vec3<float> eye, Vec3<float> target, Vec3<float> up)
	{
        Mat4 result = { 0 };

        float length = 0.0f;
        float ilength = 0.0f;

        // Vector3Subtract(eye, target)
        Vec3<double> vz = { eye.x - target.x, eye.y - target.y, eye.z - target.z };

        // Vector3Normalize(vz)
        Vec3<double> v = vz;
        length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        if (length == 0.0f) length = 1.0f;
        ilength = 1.0f / length;
        vz.x *= ilength;
        vz.y *= ilength;
        vz.z *= ilength;

        Vec3<double> vx = { up.y * vz.z - up.z * vz.y, up.z * vz.x - up.x * vz.z, up.x * vz.y - up.y * vz.x };

        v = vx;
        length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
        if (length == 0.0f) length = 1.0f;
        ilength = 1.0f / length;
        vx.x *= ilength;
        vx.y *= ilength;
        vx.z *= ilength;

        Vec3<double> vy = { vz.y * vx.z - vz.z * vx.y, vz.z * vx.x - vz.x * vx.z, vz.x * vx.y - vz.y * vx.x };

        result.m0 = vx.x;
        result.m1 = vy.x;
        result.m2 = vz.x;
        result.m3 = 0.0f;
        result.m4 = vx.y;
        result.m5 = vy.y;
        result.m6 = vz.y;
        result.m7 = 0.0f;
        result.m8 = vx.z;
        result.m9 = vy.z;
        result.m10 = vz.z;
        result.m11 = 0.0f;
        result.m12 = -(vx.x * eye.x + vx.y * eye.y + vx.z * eye.z);   // Vector3DotProduct(vx, eye)
        result.m13 = -(vy.x * eye.x + vy.y * eye.y + vy.z * eye.z);   // Vector3DotProduct(vy, eye)
        result.m14 = -(vz.x * eye.x + vz.y * eye.y + vz.z * eye.z);   // Vector3DotProduct(vz, eye)
        result.m15 = 1.0f;

        return result;
	}
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const Mat4<T>& mat)
{
    os << mat.m0 << ", " << mat.m4 << ", " << mat.m8 << ", " << mat.m12 << "\n"
        << mat.m1 << ", " << mat.m5 << ", " << mat.m9 << ", " << mat.m13 << "\n"
        << mat.m2 << ", " << mat.m6 << ", " << mat.m10 << ", " << mat.m14 << "\n"
        << mat.m3 << ", " << mat.m7 << ", " << mat.m11 << ", " << mat.m15 << "\n";
    return os;
}