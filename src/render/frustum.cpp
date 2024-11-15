#include "frustum.h"
#include "../utility/mat.h"
#include <rlgl.h>

static Mat4<double> convertMat4(const Matrix& mat)
{
    Mat4<double> m { 0.0 };
    m.m0 = mat.m0;
    m.m1 = mat.m1;
    m.m2 = mat.m2;
    m.m3 = mat.m3;
    m.m4 = mat.m4;
    m.m5 = mat.m5;
    m.m6 = mat.m6;
    m.m7 = mat.m7;
    m.m8 = mat.m8;
    m.m9 = mat.m9;
    m.m10 = mat.m10;
    m.m11 = mat.m11;
    m.m12 = mat.m12;
    m.m13 = mat.m13;
    m.m14 = mat.m14;
    m.m15 = mat.m15;
    return m;
}

static void normalizePlane(Vec4<double>& plane)
{
    double magnitude = sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);

    plane.x /= magnitude;
    plane.y /= magnitude;
    plane.z /= magnitude;
    plane.w /= magnitude;
}

static Mat4<double> matrixMultiply(Mat4<double> left, Mat4<double> right)
{
    Mat4<double> result = { 0 };

    result.m0 = left.m0 * right.m0 + left.m1 * right.m4 + left.m2 * right.m8 + left.m3 * right.m12;
    result.m1 = left.m0 * right.m1 + left.m1 * right.m5 + left.m2 * right.m9 + left.m3 * right.m13;
    result.m2 = left.m0 * right.m2 + left.m1 * right.m6 + left.m2 * right.m10 + left.m3 * right.m14;
    result.m3 = left.m0 * right.m3 + left.m1 * right.m7 + left.m2 * right.m11 + left.m3 * right.m15;
    result.m4 = left.m4 * right.m0 + left.m5 * right.m4 + left.m6 * right.m8 + left.m7 * right.m12;
    result.m5 = left.m4 * right.m1 + left.m5 * right.m5 + left.m6 * right.m9 + left.m7 * right.m13;
    result.m6 = left.m4 * right.m2 + left.m5 * right.m6 + left.m6 * right.m10 + left.m7 * right.m14;
    result.m7 = left.m4 * right.m3 + left.m5 * right.m7 + left.m6 * right.m11 + left.m7 * right.m15;
    result.m8 = left.m8 * right.m0 + left.m9 * right.m4 + left.m10 * right.m8 + left.m11 * right.m12;
    result.m9 = left.m8 * right.m1 + left.m9 * right.m5 + left.m10 * right.m9 + left.m11 * right.m13;
    result.m10 = left.m8 * right.m2 + left.m9 * right.m6 + left.m10 * right.m10 + left.m11 * right.m14;
    result.m11 = left.m8 * right.m3 + left.m9 * right.m7 + left.m10 * right.m11 + left.m11 * right.m15;
    result.m12 = left.m12 * right.m0 + left.m13 * right.m4 + left.m14 * right.m8 + left.m15 * right.m12;
    result.m13 = left.m12 * right.m1 + left.m13 * right.m5 + left.m14 * right.m9 + left.m15 * right.m13;
    result.m14 = left.m12 * right.m2 + left.m13 * right.m6 + left.m14 * right.m10 + left.m15 * right.m14;
    result.m15 = left.m12 * right.m3 + left.m13 * right.m7 + left.m14 * right.m11 + left.m15 * right.m15;

    return result;
}


Frustum::Frustum(double x, double y, double z)
{
    extract(x, y, z);
}

bool Frustum::aabbIn(const AABB& bb) const
{
    for (auto& plane : m_planes)
    {
        int out = 0;

        out += ((plane.second.x * bb.minX + plane.second.y * bb.minY + plane.second.z * bb.minZ + plane.second.w) < 0)
            ? 1 : 0;
        
        out += ((plane.second.x * bb.maxX + plane.second.y * bb.minY + plane.second.z * bb.minZ + plane.second.w) < 0)
            ? 1 : 0;
        
        out += ((plane.second.x * bb.minX + plane.second.y * bb.maxY + plane.second.z * bb.minZ + plane.second.w) < 0)
            ? 1 : 0;
        
        out += ((plane.second.x * bb.maxX + plane.second.y * bb.maxY + plane.second.z * bb.minZ + plane.second.w) < 0)
            ? 1 : 0;
        
        out += ((plane.second.x * bb.minX + plane.second.y * bb.minY + plane.second.z * bb.maxZ + plane.second.w) < 0)
            ? 1 : 0;
        
        out += ((plane.second.x * bb.maxX + plane.second.y * bb.minY + plane.second.z * bb.maxZ + plane.second.w) < 0)
            ? 1 : 0;
        
        out += ((plane.second.x * bb.minX + plane.second.y * bb.maxY + plane.second.z * bb.maxZ + plane.second.w) < 0)
            ? 1 : 0;
        
        out += ((plane.second.x * bb.maxX + plane.second.y * bb.maxY + plane.second.z * bb.maxZ + plane.second.w) < 0)
            ? 1 : 0;
        
        if (out == 8)
            return false;
    }

    return true;
}

void Frustum::extract(double x, double y, double z)
{
    Mat4<double> projection = convertMat4(rlGetMatrixProjection());
    Mat4<double> modelview = convertMat4(rlGetMatrixModelview());

    modelview = matrixMultiply({
        1.0, 0.0, 0.0, static_cast<float>(-x),
        0.0, 1.0, 0.0, static_cast<float>(-y),
        0.0, 0.0, 1.0, static_cast<float>(-z),
        0.0, 0.0, 0.0, 1.0
    }, modelview);

    Mat4<double> planes = { 0.0 };

    planes.m0 = modelview.m0 * projection.m0 + modelview.m1 * projection.m4 + modelview.m2 * projection.m8 + modelview.m3 * projection.m12;
    planes.m1 = modelview.m0 * projection.m1 + modelview.m1 * projection.m5 + modelview.m2 * projection.m9 + modelview.m3 * projection.m13;
    planes.m2 = modelview.m0 * projection.m2 + modelview.m1 * projection.m6 + modelview.m2 * projection.m10 + modelview.m3 * projection.m14;
    planes.m3 = modelview.m0 * projection.m3 + modelview.m1 * projection.m7 + modelview.m2 * projection.m11 + modelview.m3 * projection.m15;
    planes.m4 = modelview.m4 * projection.m0 + modelview.m5 * projection.m4 + modelview.m6 * projection.m8 + modelview.m7 * projection.m12;
    planes.m5 = modelview.m4 * projection.m1 + modelview.m5 * projection.m5 + modelview.m6 * projection.m9 + modelview.m7 * projection.m13;
    planes.m6 = modelview.m4 * projection.m2 + modelview.m5 * projection.m6 + modelview.m6 * projection.m10 + modelview.m7 * projection.m14;
    planes.m7 = modelview.m4 * projection.m3 + modelview.m5 * projection.m7 + modelview.m6 * projection.m11 + modelview.m7 * projection.m15;
    planes.m8 = modelview.m8 * projection.m0 + modelview.m9 * projection.m4 + modelview.m10 * projection.m8 + modelview.m11 * projection.m12;
    planes.m9 = modelview.m8 * projection.m1 + modelview.m9 * projection.m5 + modelview.m10 * projection.m9 + modelview.m11 * projection.m13;
    planes.m10 = modelview.m8 * projection.m2 + modelview.m9 * projection.m6 + modelview.m10 * projection.m10 + modelview.m11 * projection.m14;
    planes.m11 = modelview.m8 * projection.m3 + modelview.m9 * projection.m7 + modelview.m10 * projection.m11 + modelview.m11 * projection.m15;
    planes.m12 = modelview.m12 * projection.m0 + modelview.m13 * projection.m4 + modelview.m14 * projection.m8 + modelview.m15 * projection.m12;
    planes.m13 = modelview.m12 * projection.m1 + modelview.m13 * projection.m5 + modelview.m14 * projection.m9 + modelview.m15 * projection.m13;
    planes.m14 = modelview.m12 * projection.m2 + modelview.m13 * projection.m6 + modelview.m14 * projection.m10 + modelview.m15 * projection.m14;
    planes.m15 = modelview.m12 * projection.m3 + modelview.m13 * projection.m7 + modelview.m14 * projection.m11 + modelview.m15 * projection.m15;

    m_planes[Planes::RIGHT] = { planes.m3 - planes.m0, planes.m7 - planes.m4, planes.m11 - planes.m8, planes.m15 - planes.m12 };
    normalizePlane(m_planes[Planes::RIGHT]);

    m_planes[Planes::LEFT] = { planes.m3 + planes.m0, planes.m7 + planes.m4, planes.m11 + planes.m8, planes.m15 + planes.m12 };
    normalizePlane(m_planes[Planes::LEFT]);

    m_planes[Planes::TOP] = { planes.m3 - planes.m1, planes.m7 - planes.m5, planes.m11 - planes.m9, planes.m15 - planes.m13 };
    normalizePlane(m_planes[Planes::TOP]);

    m_planes[Planes::BOTTOM] = { planes.m3 + planes.m1, planes.m7 + planes.m5, planes.m11 + planes.m9, planes.m15 + planes.m13 };
    normalizePlane(m_planes[Planes::BOTTOM]);

    m_planes[Planes::BACK] = { planes.m3 - planes.m2, planes.m7 - planes.m6, planes.m11 - planes.m10, planes.m15 - planes.m14 };
    normalizePlane(m_planes[Planes::BACK]);

    m_planes[Planes::FRONT] = { planes.m3 + planes.m2, planes.m7 + planes.m6, planes.m11 + planes.m10, planes.m15 + planes.m14 };
    normalizePlane(m_planes[Planes::FRONT]);
}
