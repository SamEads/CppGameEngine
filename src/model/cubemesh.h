#pragma once

#include <array>
#include <vector>
#include "../utility/vec.h"
#include "../utility/col.h"
#include "../memory/pointers.h"
#include "../graphics/tex2d.h"

// typedef struct Mesh;
class Cam3D;

class CubeMesh
{
public:
	struct Data
	{
		unsigned int vaoId;			// Vertex Array Object id
		unsigned int* vboId;		// Vertex Buffer Objects id (default vertex data)

		int vertexCount;			// Number of vertices stored in arrays
		int triangleCount;			// Number of triangles stored (indexed or not)

		float* vertices;			// Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
		float* texcoords;			// Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
		float* normals;				// Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
		unsigned char* colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
		unsigned short* indices;    // Vertex indices (in case vertex data comes indexed)
	};
private:
	Data m_data { 0 };

	struct Face
	{
		struct Vertex
		{
			float x, y, z, u, v;
		};
		std::array<Vertex, 4> vertices { };
	};
	std::vector<Face> m_faces;

	Vec2<int> m_texSize { 0 };
	Vec2<int> m_texOffset { 0 };

	bool m_compiled = false;

	bool m_mirrored = false;

	void addFace(Face face, int u1, int v1, int u2, int v2);

	void flipFaces();

	void upload(float scale);

	void unload() const;

	static Vec3<float> calculateNormal(const CubeMesh::Face& face);

public:
	CubeMesh() = default;

	Vec3<float> rotationPoint { 0 };
	Vec3<float> rotationAngle { 0 };

	void init(Vec2<int> texSize, Vec2<int> texOffset, bool mirrored = false);
	
	void setRotationPoint(Vec3<float> point);

	void addBox(Vec3<float> min, Vec3<int> size, float extrusion = 0.0f);

	void render(Cam3D* camera, const Tex2D& tex, float scale);

	CubeMesh(const CubeMesh& other) = delete;

	CubeMesh& operator=(const CubeMesh& other) = delete;

	~CubeMesh();
};