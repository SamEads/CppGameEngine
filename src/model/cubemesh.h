#pragma once

#include "../utility/vec.h"
#include <cstdint>
#include <array>
#include <vector>
#include "../graphics/texture.h"

class MatrixStack;
class Shader;

class CubeMesh
{
public:
	struct Vertex
	{
		float x, y, z;
		float u, v;
		float normX, normY, normZ;
	};
private:
	unsigned int m_VBO, m_VAO, m_EBO;

	/*
	struct Face
	{
		Vertex vertices[4];
	};
	*/

	std::vector<Vertex> m_vertices;
	std::vector<unsigned int> m_indices;

	Vec2<int> m_texSize{ 0 };
	Vec2<int> m_texOffset{ 0 };

	bool m_compiled = false;

	bool m_mirrored = false;

	// void addFace(Face face, int u1, int v1, int u2, int v2);

	void flipFaces();

	void unload() const;

	// static Vec3<float> calculateNormal(const CubeMesh::Face& face);

public:
	CubeMesh() = default;

	Vec3<float> rotationPoint{ 0 };
	Vec3<float> rotationAngle{ 0 };

	void init(Vec2<int> texSize, Vec2<int> texOffset, bool mirrored = false);

	void upload(float scale);

	void setRotationPoint(const Vec3<float>& point);

	void addBox(Vec3<float> min, Vec3<int> size, float extrusion = 0.0f);

	void render(MatrixStack* matrix, Shader* shader, Texture* texture, float scale);
	void render(MatrixStack* matrix, Shader* shader, float scale);

	CubeMesh(const CubeMesh& other) = delete;

	CubeMesh& operator=(const CubeMesh& other) = delete;

	~CubeMesh();
};