// https://gist.github.com/podgorskiy/e698d18879588ada9014768e3e82a644

#pragma once 

#include <matrix.hpp>
#include "../physics/aabb.h"
#include "camera3d.h"
#include <map>

class Frustum
{
public:
	Frustum(const Camera3D& camera);

	// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
	bool aabbIn(const AABB& aabb) const;

private:
	enum class Planes
	{
		LEFT = 0,
		RIGHT,
		BOTTOM,
		TOP,
		NEAR,
		FAR
	};

	std::map<Planes, glm::vec4> m_planes;
	glm::vec3 m_points[8];

	// i * (9 - i) / 2 + j - 1
	template<Planes i, Planes j>
	static size_t ij2k()
	{
		return static_cast<int>(i) * (9 - static_cast<int>(i)) / 2 + static_cast<int>(j) - 1;
	};

	template<Planes a, Planes b, Planes c>
	glm::vec3 intersection(const glm::vec3* crosses) const
	{
		float dot = glm::dot(glm::vec3(m_planes.at(a)), crosses[ij2k<b, c>()]);

		glm::vec3 res = glm::mat3(crosses[ij2k<b, c>()], -crosses[ij2k<a, c>()], crosses[ij2k<a, b>()]) *
			glm::vec3(m_planes.at(a).w, m_planes.at(b).w, m_planes.at(c).w);

		return res * (-1.0f / dot);
	}
};