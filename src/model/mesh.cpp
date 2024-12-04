#include "mesh.h"
#include "../utility/vec.h"
#include <glad/glad.h>

void MeshBase::unload()
{
	if (m_loaded)
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(1, &m_VBO);
		glDeleteBuffers(1, &m_EBO);
		m_loaded = false;
	}
}

MeshBase::~MeshBase()
{
	unload();
}

void MeshBase::drawElements(const Texture& texture)
{
	glBindVertexArray(m_VAO);
	texture.bind(0);

	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

	texture.unbind();
	glBindVertexArray(0);
}

void Mesh::calculateNormals()
{
	for (int i = 0; i < m_vertices.size(); i += 4)
	{
		std::vector<Vertex> face = std::vector<Vertex>(m_vertices.begin() + i, m_vertices.begin() + i + 4);

		Vec3<float> v1 = { face[1].x - face[0].x, face[1].y - face[0].y, face[1].z - face[0].z };
		Vec3<float> v2 = { face[3].x - face[0].x, face[3].y - face[0].y, face[3].z - face[0].z };
		auto normal = v1.cross(v2).normalize();

		for (auto& v : face)
		{
			v.normX = normal.x;
			v.normY = normal.y;
			v.normZ = normal.z;
		}
	}
}

void Mesh::calculateIndices()
{
	m_indices.resize((m_vertices.size() / 4) * 6);

	static const int indicesMap[6] = {
		0, 1, 2,
		0, 2, 3
	};

	for (int i = 0; i < m_indices.size(); i++)
	{
		m_indices[i] = ((i / 6) * 4) + indicesMap[i % 6];
	}
}

void Mesh::vertexUV(float x, float y, float z, float u, float v)
{
	m_vertices.emplace_back(x, y, z, u, v);
}