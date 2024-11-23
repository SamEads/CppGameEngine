#pragma once

#include "../utility/vec.h"
#include <cstdint>
#include <array>
#include <vector>
#include "../graphics/texture.h"

class MatrixStack;
class Shader;

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

	void vertexUVNormal(float x, float y, float z, float u, float v, float nx, float ny, float nz);

	virtual ~Mesh() override = default;
};

class CubeMesh : public Mesh
{
private:
	Vec2<int> m_texSize { 0 };
	Vec2<int> m_texOffset { 0 };

	bool m_compiled = false;

	bool m_mirrored = false;

	void flipFaces();

public:
	CubeMesh() = default;

	Vec3<float> rotationPoint { 0 };
	Vec3<float> rotationAngle { 0 };

	void init(Vec2<int> texSize, Vec2<int> texOffset, bool mirrored = false);

	void upload(float scale);

	void setRotationPoint(const Vec3<float>& point);

	void addBox(Vec3<float> min, Vec3<int> size, float extrusion = 0.0f);

	void render(MatrixStack* matrix, Shader* shader, Texture* texture, float scale);
	void render(MatrixStack* matrix, Shader* shader, float scale);

	CubeMesh(const CubeMesh& other) = delete;

	CubeMesh& operator=(const CubeMesh& other) = delete;
};