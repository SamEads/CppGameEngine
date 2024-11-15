#include <raylib.h>
#include <raymath.h>

#include "cubemesh.h"
#include "../render/cam3d.h"
#include "../../external/glad.h"

static Mesh punMesh(const CubeMesh::Data& data)
{
	Mesh mesh { 0 };

	mesh.vaoId = data.vaoId;
	mesh.vboId = data.vboId;

	mesh.vertexCount = data.vertexCount;
	mesh.triangleCount = data.triangleCount;

	mesh.vertices = data.vertices;
	mesh.texcoords = data.texcoords;
	mesh.normals = data.normals;
	mesh.colors = data.colors;
	mesh.indices = data.indices;

	return mesh;
}

static void unpunMesh(const Mesh& mesh, CubeMesh::Data& data)
{
	data.vaoId = mesh.vaoId;
	data.vboId = mesh.vboId;
}

const int quadToTriMap[6] = { 0, 1, 2, 0, 2, 3 };
Vec3<float> CubeMesh::calculateNormal(const CubeMesh::Face& face)
{
	Vec3<float> v1 = { face.vertices[1].x - face.vertices[0].x, face.vertices[1].y - face.vertices[0].y, face.vertices[1].z - face.vertices[0].z };
	Vec3<float> v2 = { face.vertices[3].x - face.vertices[0].x, face.vertices[3].y - face.vertices[0].y, face.vertices[3].z - face.vertices[0].z };
	return v1.cross(v2).normalize();
}

void CubeMesh::init(Vec2<int> texSize, Vec2<int> texOffset, bool mirrored)
{
	m_faces.clear();

	m_texSize = texSize;
	m_texOffset = texOffset;

	m_mirrored = mirrored;
}

void CubeMesh::setRotationPoint(Vec3<float> point)
{
	rotationPoint = point;
}

void CubeMesh::flipFaces()
{
	for (auto& face : m_faces)
	{
		Face newFace;
		for (int i = 0; i < face.vertices.size(); ++i)
		{
			newFace.vertices[i] = face.vertices[face.vertices.size() - i - 1];
		}
		face = newFace;
	}
}

void CubeMesh::addBox(Vec3<float> min, Vec3<int> size, float extrusion)
{
	Vec3<float> max = {
		min.x + static_cast<float>(size.x),
		min.y + static_cast<float>(size.y),
		min.z + static_cast<float>(size.z)
	};

	min.x -= extrusion;
	min.y -= extrusion;
	min.z -= extrusion;

	max.x += extrusion;
	max.y += extrusion;
	max.z += extrusion;

	if (m_mirrored)
	{
		float temp = max.x;
		max.x = min.x;
		min.x = temp;
	}

	Face::Vertex bottomFrontLeft	{ min.x, min.y, min.z };
	Face::Vertex bottomFrontRight	{ max.x, min.y, min.z };
	Face::Vertex topFrontRight		{ max.x, max.y, min.z };
	Face::Vertex topFrontLeft		{ min.x, max.y, min.z };
	Face::Vertex bottomBackLeft		{ min.x, min.y, max.z };
	Face::Vertex bottomBackRight	{ max.x, min.y, max.z };
	Face::Vertex topBackRight		{ max.x, max.y, max.z };
	Face::Vertex topBackLeft		{ min.x, max.y, max.z };

	addFace(
		{ bottomBackRight, bottomFrontRight, topFrontRight, topBackRight },
		m_texOffset.x + size.z + size.x, m_texOffset.y + size.z, m_texOffset.x + size.z + size.x + size.z, m_texOffset.y + size.z + size.y
	);

	addFace(
		{ bottomFrontLeft, bottomBackLeft, topBackLeft, topFrontLeft },
		m_texOffset.x + 0, m_texOffset.y + size.z, m_texOffset.x + size.z, m_texOffset.y + size.z + size.y
	);

	addFace(
		{ bottomBackRight, bottomBackLeft, bottomFrontLeft, bottomFrontRight },
		m_texOffset.x + size.z, m_texOffset.y + 0, m_texOffset.x + size.z + size.x, m_texOffset.y + size.z
	);

	addFace(
		{ topFrontRight, topFrontLeft, topBackLeft, topBackRight },
		m_texOffset.x + size.z + size.x, m_texOffset.y + 0, m_texOffset.x + size.z + size.x + size.x, m_texOffset.y + size.z
	);

	addFace(
		{ bottomFrontRight, bottomFrontLeft, topFrontLeft, topFrontRight },
		m_texOffset.x + size.z, m_texOffset.y + size.z, m_texOffset.x + size.z + size.x, m_texOffset.y + size.z + size.y
	);

	addFace(
		{ bottomBackLeft, bottomBackRight, topBackRight, topBackLeft },
		m_texOffset.x + size.z + size.x + size.z, m_texOffset.y + size.z, m_texOffset.x + size.z + size.x + size.z + size.x, m_texOffset.y + size.z + size.y
	);
	
	if (m_mirrored)
	{
		flipFaces();
	}
}

void CubeMesh::addFace(Face face, int u1, int v1, int u2, int v2)
{
	float texelW = 0.1f / m_texSize.x;
	float texelH = 0.1f / m_texSize.y;

	face.vertices[0].u = (static_cast<float>(u2) / m_texSize.x) - texelW;
	face.vertices[0].v = (static_cast<float>(v1) / m_texSize.y) + texelH;

	face.vertices[1].u = (static_cast<float>(u1) / m_texSize.x) + texelW;
	face.vertices[1].v = (static_cast<float>(v1) / m_texSize.y) + texelH;

	face.vertices[2].u = (static_cast<float>(u1) / m_texSize.x) + texelW;
	face.vertices[2].v = (static_cast<float>(v2) / m_texSize.y) - texelH;

	face.vertices[3].u = (static_cast<float>(u2) / m_texSize.x) - texelW;
	face.vertices[3].v = (static_cast<float>(v2) / m_texSize.y) - texelH;

	m_faces.push_back(face);
}

void CubeMesh::render(Cam3D* camera, const Tex2D& tex, float scale)
{
	if (!m_compiled)
	{
		upload(scale);	
	}

	camera->pushMatrix();
	camera->translate(rotationPoint.x * scale, rotationPoint.y * scale, rotationPoint.z * scale);
	
	if (rotationAngle.z != 0.f) camera->rotate(rotationAngle.z * 180.f / PI, 0.0f, 0.0f, 1.0f);
	if (rotationAngle.y != 0.f) camera->rotate(rotationAngle.y * 180.f / PI, 0.0f, 1.0f, 0.0f);
	if (rotationAngle.x != 0.f) camera->rotate(rotationAngle.x * 180.f / PI, 1.0f, 0.0f, 0.0f);
	
#pragma region Raylib rendering

	static Material cubeMat;
	if (cubeMat.shader.id == 0)
	{
		cubeMat = LoadMaterialDefault();
	}

	auto& matTex = cubeMat.maps[MATERIAL_MAP_DIFFUSE].texture;
	matTex.id = tex.getId();
	matTex.width = tex.getSize().x;
	matTex.height = tex.getSize().y;
	matTex.format = tex.getFormat();

	DrawMesh(punMesh(m_data), cubeMat, MatrixIdentity());

#pragma endregion

	camera->popMatrix();
}

void CubeMesh::upload(float scale)
{
	// Create mesh
	m_data = { 0 };

	m_data.triangleCount = std::ceil(m_faces.size() * 2);
	m_data.vertexCount = m_data.triangleCount * 3;
	
	// Set up memory in mesh
	m_data.vertices = new float[m_data.vertexCount * 3 * sizeof(float)];
	m_data.indices = new unsigned short[m_data.triangleCount * 3 * sizeof(unsigned short)];
	m_data.texcoords = new float[m_data.vertexCount * 2 * sizeof(float)];
	m_data.normals = new float[m_data.vertexCount * 3 * sizeof(float)];

	int vertices = 0, normals = 0, texcoords = 0, indices = 0;
	for (const auto& face : m_faces)
	{
		Vec3<float> normal = calculateNormal(face);
		
		// used for calculating indices.
		int base = vertices / 3;

		// calculate vertices / normals / texcoords
		for (int i = 0; i < 4; ++i)
		{
			m_data.vertices[vertices++] = face.vertices[i].x * scale;
			m_data.vertices[vertices++] = face.vertices[i].y * scale;
			m_data.vertices[vertices++] = face.vertices[i].z * scale;

			m_data.normals[normals++] = normal.x;
			m_data.normals[normals++] = normal.y;
			m_data.normals[normals++] = normal.z;
			
			m_data.texcoords[texcoords++] = face.vertices[i].u;
			m_data.texcoords[texcoords++] = face.vertices[i].v;
		}

		// calculate indices
		for (int i = 0; i < 6; i++)
		{
			m_data.indices[indices++] = base + quadToTriMap[i];
		}
	}

	// upload mesh to GPU
	{
		Mesh punnedMesh = punMesh(m_data);
		UploadMesh(&punnedMesh, false);
		unpunMesh(punnedMesh, m_data);
	}

	m_compiled = true;

	std::cout << "[INFO] Uploaded mesh\n";
}

void CubeMesh::unload() const
{
	if (!m_compiled)
	{
		return;
	}

	if (m_data.vboId != nullptr)
	{
		for (int i = 0; i < 7 /* max mesh vertex buffers */; ++i)
		{
			glDeleteBuffers(1, &m_data.vboId[i]);
		}
	}

	delete[] m_data.vertices;
	delete[] m_data.texcoords;
	delete[] m_data.normals;
	delete[] m_data.colors;
	delete[] m_data.indices;
}

CubeMesh::~CubeMesh()
{
	unload();
}