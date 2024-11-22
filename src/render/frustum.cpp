#include "frustum.h"

constexpr int PLANE_COUNT = 6;
constexpr int PLANE_COMBINATIONS = PLANE_COUNT * (PLANE_COUNT - 1) / 2;

Frustum::Frustum(const Camera3D& camera)
{
	auto m = glm::transpose(camera.getProjection() * camera.getView());

	m_planes[Planes::LEFT] =	m[3] + m[0];
	m_planes[Planes::RIGHT] =	m[3] - m[0];
	m_planes[Planes::BOTTOM] =	m[3] + m[1];
	m_planes[Planes::TOP] =		m[3] - m[1];
	m_planes[Planes::NEAR] =	m[3] + m[2];
	m_planes[Planes::FAR] =		m[3] - m[2];

	glm::vec3 crosses[PLANE_COMBINATIONS] =
	{
		glm::cross(glm::vec3(m_planes[Planes::LEFT]),   glm::vec3(m_planes[Planes::RIGHT])),
		glm::cross(glm::vec3(m_planes[Planes::LEFT]),   glm::vec3(m_planes[Planes::BOTTOM])),
		glm::cross(glm::vec3(m_planes[Planes::LEFT]),   glm::vec3(m_planes[Planes::TOP])),
		glm::cross(glm::vec3(m_planes[Planes::LEFT]),   glm::vec3(m_planes[Planes::NEAR])),
		glm::cross(glm::vec3(m_planes[Planes::LEFT]),   glm::vec3(m_planes[Planes::FAR])),
		glm::cross(glm::vec3(m_planes[Planes::RIGHT]),  glm::vec3(m_planes[Planes::BOTTOM])),
		glm::cross(glm::vec3(m_planes[Planes::RIGHT]),  glm::vec3(m_planes[Planes::TOP])),
		glm::cross(glm::vec3(m_planes[Planes::RIGHT]),  glm::vec3(m_planes[Planes::NEAR])),
		glm::cross(glm::vec3(m_planes[Planes::RIGHT]),  glm::vec3(m_planes[Planes::FAR])),
		glm::cross(glm::vec3(m_planes[Planes::BOTTOM]), glm::vec3(m_planes[Planes::TOP])),
		glm::cross(glm::vec3(m_planes[Planes::BOTTOM]), glm::vec3(m_planes[Planes::NEAR])),
		glm::cross(glm::vec3(m_planes[Planes::BOTTOM]), glm::vec3(m_planes[Planes::FAR])),
		glm::cross(glm::vec3(m_planes[Planes::TOP]),    glm::vec3(m_planes[Planes::NEAR])),
		glm::cross(glm::vec3(m_planes[Planes::TOP]),    glm::vec3(m_planes[Planes::FAR])),
		glm::cross(glm::vec3(m_planes[Planes::NEAR]),   glm::vec3(m_planes[Planes::FAR]))
	};

	m_points[0] = intersection<Planes::LEFT, Planes::BOTTOM, Planes::NEAR>(crosses);
	m_points[1] = intersection<Planes::LEFT, Planes::TOP, Planes::NEAR>(crosses);
	m_points[2] = intersection<Planes::RIGHT, Planes::BOTTOM, Planes::NEAR>(crosses);
	m_points[3] = intersection<Planes::RIGHT, Planes::TOP, Planes::NEAR>(crosses);
	m_points[4] = intersection<Planes::LEFT, Planes::BOTTOM, Planes::FAR>(crosses);
	m_points[5] = intersection<Planes::LEFT, Planes::TOP, Planes::FAR>(crosses);
	m_points[6] = intersection<Planes::RIGHT, Planes::BOTTOM, Planes::FAR>(crosses);
	m_points[7] = intersection<Planes::RIGHT, Planes::TOP, Planes::FAR>(crosses);

}

// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
bool Frustum::aabbIn(const AABB& aabb) const
{
	// check box outside/inside of frustum
	for (int i = 0; i < PLANE_COUNT; i++)
	{
		if ((glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.minX, aabb.minY, aabb.minZ, 1.0f)) < 0.0) &&
			(glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.maxX, aabb.minY, aabb.minZ, 1.0f)) < 0.0) &&
			(glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.minX, aabb.maxY, aabb.minZ, 1.0f)) < 0.0) &&
			(glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.maxX, aabb.maxY, aabb.minZ, 1.0f)) < 0.0) &&
			(glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.minX, aabb.minY, aabb.maxZ, 1.0f)) < 0.0) &&
			(glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.maxX, aabb.minY, aabb.maxZ, 1.0f)) < 0.0) &&
			(glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.minX, aabb.maxY, aabb.maxZ, 1.0f)) < 0.0) &&
			(glm::dot(m_planes.at(static_cast<Planes>(i)), glm::vec4(aabb.maxX, aabb.maxY, aabb.maxZ, 1.0f)) < 0.0))
		{
			return false;
		}
	}

	// check frustum outside/inside box
	int out;
	out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x > aabb.maxX) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].x < aabb.minX) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y > aabb.maxY) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].y < aabb.minY) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z > aabb.maxZ) ? 1 : 0); if (out == 8) return false;
	out = 0; for (int i = 0; i < 8; i++) out += ((m_points[i].z < aabb.minZ) ? 1 : 0); if (out == 8) return false;

	return true;
}