#include "camera3d.h"
#include "../utility/mat.h"
#include "../utility/defines.h"
#include <ext/matrix_clip_space.hpp>
#include <ext/matrix_transform.hpp>
#include "window.h"

Camera3D::Camera3D(Window* window, float fov, int perspective) :
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