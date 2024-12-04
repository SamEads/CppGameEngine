#pragma once
#include <vector>
#include "../graphics/texture.h"

class MeshBase
{
protected:
	unsigned int m_VBO, m_VAO, m_EBO;
	bool m_loaded = false;

	void unload();

	std::vector<unsigned int> m_indices;

	virtual ~MeshBase();

	void drawElements(const Texture& texture);
};

class Mesh : public MeshBase
{
protected:
	struct Vertex
	{
		float x, y, z;
		float u, v;
		float normX, normY, normZ;
		Vertex(float x, float y, float z, float u, float v) :
			x(x), y(y), z(z), u(u), v(v), normX(0.0f), normY(0.0f), normZ(0.0f) {}
		Vertex(float x, float y, float z, float u, float v, float normX, float normY, float normZ) :
			x(x), y(y), z(z), u(u), v(v), normX(normX), normY(normY), normZ(normZ) {}
	};

	std::vector<Vertex> m_vertices;

	void calculateNormals();

	void calculateIndices();

	void vertexUV(float x, float y, float z, float u, float v);

public:
	virtual ~Mesh() override = default;
};