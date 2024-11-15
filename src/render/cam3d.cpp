#include "cam3d.h"
#include <raylib.h>
#include <rlgl.h>
#include "../utility/mat.h"
#include "../../external/glad.h"

Cam3D::Cam3D(float fov, int perspective) :
    m_position({ 0.0f, 0.0f, 0.0f }),
    m_target({ 0.0f, 0.0f, -1.0f }),
    m_up({ 0.0f, 1.0f, 0.0f }),
    m_fov(fov),
    m_proj(perspective),
    m_matrixPushes(0) {}

constexpr auto NEAR_PLANE = 0.05;
void Cam3D::beginDrawing(const Vec2<int> screenSize, float farPlane)
{
    rlDrawRenderBatchActive();

    rlMatrixMode(RL_PROJECTION);
    rlPushMatrix();
    rlLoadIdentity();

    float aspect = screenSize.x / static_cast<float>(screenSize.y);

    // zNear and zFar values are important when computing depth buffer values
    if (m_proj == CAMERA_PERSPECTIVE)
    {
        // Setup perspective projection
        double top = NEAR_PLANE * tan(m_fov * 0.5 * DEG2RAD);
        double right = top * aspect;

        rlFrustum(-right, right, -top, top, NEAR_PLANE, farPlane);
    }
    else if (m_proj == CAMERA_ORTHOGRAPHIC)
    {
        // Setup orthographic projection
        double top = m_fov / 2.0;
        double right = top * aspect;

        rlOrtho(-right, right, -top, top, NEAR_PLANE, farPlane);
    }

    rlMatrixMode(RL_MODELVIEW);     // Switch back to modelview matrix
    rlLoadIdentity();               // Reset current matrix (modelview)

    // Setup Camera view
    Mat4<float> matView = Mat4<float>::lookAt(m_position, m_target, m_up);

    auto floatMat = matView.to16();
    rlMultMatrixf(floatMat.data());

    rlEnableDepthTest();
}

void Cam3D::endDrawing()
{
    if (m_matrixPushes > 0)
    {
        std::cout << "[WARNING] " << m_matrixPushes << " " << ((m_matrixPushes > 1) ? "matrices" : "matrix") << " pushed but not popped!\n";
        while (m_matrixPushes > 0)
        {
            popMatrix();
        }
    }
    EndMode3D();
}

float Cam3D::getFOV() const
{
    return m_fov;
}

void Cam3D::setFOV(float fov)
{
    m_fov = fov;
}

Vec3<float> Cam3D::getDirection() const
{
    Vec3<float> camTarget = { m_target.x, m_target.y, m_target.z };
    Vec3<float> camPos = { m_position.x, m_position.y, m_position.z };

    return Vec3<float>(camTarget - camPos).normalize();
}

void Cam3D::pushMatrix()
{
    ++m_matrixPushes;
    rlPushMatrix();
}

void Cam3D::popMatrix()
{
    --m_matrixPushes;
    rlPopMatrix();
}

void Cam3D::orient(const float yaw, const float pitch)
{
    rotate(pitch, 1.0f, 0.0f, 0.0f);
    rotate(yaw + 180.0f, 0.0f, 1.0f, 0.0f); // Camera needs to be oriented 180 degrees on yaw axis
}

/* Adapted from RL-GL impl. */
namespace CameraMatrix
{
    Mat4<float> active (Mat4<float>::identity());
    
    static void translate(float x, float y, float z)
    {
        Mat4<float> translation = {
            1.0f, 0.0f, 0.0f, x,
            0.0f, 1.0f, 0.0f, y,
            0.0f, 0.0f, 1.0f, z,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        active = translation * active;
    }

    static void scale(float x, float y, float z)
    {
        Mat4<float> scale = {
            x, 0.0f, 0.0f, 0.0f,
            0.0f, y, 0.0f, 0.0f,
            0.0f, 0.0f, z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };

        active = scale * active;
    }

    static void rotate(float angle, float x, float y, float z)
    {
        Mat4<float> rotation = Mat4<float>::identity();

        float lenSqr = x * x + y * y + z * z;
        if ((lenSqr != 1.0f) && (lenSqr != 0.0f))
        {
            float inverseLen = 1.0f / sqrtf(lenSqr);
            x *= inverseLen;
            y *= inverseLen;
            z *= inverseLen;
        }

        // Rotation matrix generation
        float sinres = sinf(DEG2RAD * angle);
        float cosres = cosf(DEG2RAD * angle);
        float t = 1.0f - cosres;

        rotation.m0 = x * x * t + cosres;
        rotation.m1 = y * x * t + z * sinres;
        rotation.m2 = z * x * t - y * sinres;
        rotation.m3 = 0.0f;

        rotation.m4 = x * y * t - z * sinres;
        rotation.m5 = y * y * t + cosres;
        rotation.m6 = z * y * t + x * sinres;
        rotation.m7 = 0.0f;

        rotation.m8 = x * z * t + y * sinres;
        rotation.m9 = y * z * t - x * sinres;
        rotation.m10 = z * z * t + cosres;
        rotation.m11 = 0.0f;

        rotation.m12 = 0.0f;
        rotation.m13 = 0.0f;
        rotation.m14 = 0.0f;
        rotation.m15 = 1.0f;

        active = rotation * active;
    }
}

void Cam3D::rotate(float angle, float x, float y, float z)
{
    rlRotatef(angle, x, y, z);
}

void Cam3D::translate(float x, float y, float z)
{
    rlTranslatef(x, y, z);
}

void Cam3D::scale(float x, float y, float z)
{
    rlScalef(x, y, z);
}

void Cam3D::clearDepthBuffer()
{
    rlDrawRenderBatchActive();
    glClear(GL_DEPTH_BUFFER_BIT);
}
