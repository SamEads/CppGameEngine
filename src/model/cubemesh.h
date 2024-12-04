#pragma once

#include "mesh.h"
#include "../utility/vec.h"

class MatrixStack;
class Shader;

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