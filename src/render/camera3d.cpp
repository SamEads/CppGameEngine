#include "camera3d.h"
#include "../utility/mat.h"
#include "../utility/defines.h"
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>

Camera3D::Camera3D(Window* window, float fov, int perspective) :
    m_position({ 0.0f, 0.0f, 0.0f }),
    m_target({ 0.0f, 0.0f, -1.0f }),
    m_up({ 0.0f, 1.0f, 0.0f }),
    m_fov(fov),
    m_proj(perspective),
    m_matrixPushes(0) {}

constexpr float NEAR_PLANE = 0.05;

void Camera3D::beginDrawing(const Vec2<int> screenSize, float farPlane)
{
    m_projM = glm::perspective(
        glm::radians(m_fov),  // field of view
        screenSize.x / static_cast<float>(screenSize.y), // aspect
        NEAR_PLANE,
        farPlane
    );

    m_viewM = glm::mat4(1.0f);
}

void Camera3D::endDrawing()
{
    if (m_matrixPushes > 0)
    {
        std::cout << "[WARNING] " << m_matrixPushes << " " << ((m_matrixPushes > 1) ? "matrices" : "matrix") << " pushed but not popped!\n";
        while (m_matrixPushes > 0)
        {
            popMatrix();
        }
    }
}

float Camera3D::getFOV() const
{
    return m_fov;
}

void Camera3D::setFOV(float fov)
{
    m_fov = fov;
}

Vec3<float> Camera3D::getDirection() const
{
    Vec3<float> camTarget = { m_target.x, m_target.y, m_target.z };
    Vec3<float> camPos = { m_position.x, m_position.y, m_position.z };

    return Vec3<float>(camTarget - camPos).normalize();
}

void Camera3D::pushMatrix()
{
    ++m_matrixPushes;
    m_modelM.push();
}

void Camera3D::popMatrix()
{
    --m_matrixPushes;
    m_modelM.pop();
}

void Camera3D::orient(const float yaw, const float pitch)
{
    m_viewM = glm::rotate(m_viewM, glm::radians(pitch), glm::vec3(1, 0, 0));        // x
    m_viewM = glm::rotate(m_viewM, glm::radians(yaw + 180.0f), glm::vec3(0, 1, 0)); // y
    m_viewM = glm::rotate(m_viewM, glm::radians(0.0f), glm::vec3(0, 0, 1));         // z
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

void Camera3D::rotate(float angle, float x, float y, float z)
{
    m_modelM.rotate(angle, x, y, z);
}

void Camera3D::translate(float x, float y, float z)
{
    m_modelM.translate(x, y, z);
}

void Camera3D::scale(float x, float y, float z)
{
    m_modelM.scale(x, y, z);
}

void Camera3D::clearDepthBuffer()
{

}

const glm::mat4& Camera3D::getView() const
{
    return m_viewM;
}

const glm::mat4& Camera3D::getProjection() const
{
    return m_projM;
}

MatrixStack* Camera3D::getModelStack()
{
    return &m_modelM;
}


const glm::mat4& Camera3D::getModel() const
{
    return m_modelM.top();
}