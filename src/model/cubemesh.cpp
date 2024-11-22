#include "cubemesh.h"
#include "../utility/matrixstack.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include <glad/glad.h>
#include "../utility/defines.h"

void CubeMesh::flipFaces()
{
	int faces = m_vertices.size() / 4;
	for (int i = 0; i < faces; ++i)
	{
		std::vector<Vertex> faceCopy(
			m_vertices.begin() + (i * 4),
			m_vertices.begin() + (i * 4) + 4
		);
		m_vertices[(i * 4)] = faceCopy[3];
		m_vertices[(i * 4) + 1] = faceCopy[2];
		m_vertices[(i * 4) + 2] = faceCopy[1];
		m_vertices[(i * 4) + 3] = faceCopy[0];
	}
}

static void checkGLErr(const std::string& location)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error at " << location << ": " << err << std::endl;
	}
}

void CubeMesh::upload(float scale)
{
	if (m_vertices.empty() || m_vertices.size() % 4 != 0)
	{
		return;
	}

	size_t faceCount = m_vertices.size() / 4;

	std::cout << "Face count: " << faceCount << "\n";

	m_indices.resize(faceCount * 6);

	std::cout << "Indices count: " << m_indices.size() << "\n";

	glGenVertexArrays(1, &m_VAO);
	checkGLErr("glGenVertexArrays");
	glGenBuffers(1, &m_VBO);
	checkGLErr("glGenBuffers VBO");
	glGenBuffers(1, &m_EBO);
	checkGLErr("glGenBuffers EBO");

	glBindVertexArray(m_VAO);
	checkGLErr("glBindVertexArray");

	// Vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	checkGLErr("glBindBuffer GL_ARRAY_BUFFER");

	// Apply scaling
	for (auto& v : m_vertices)
	{
		v.x *= scale;
		v.y *= scale;
		v.z *= scale;
	}

	// Calculate normals
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

	// Calculate indices
	static const int indicesMap[6] = {
		0, 1, 2,
		0, 2, 3
	};
	for (int i = 0; i < m_indices.size(); i++)
	{
		m_indices[i] = ((i / 6) * 4) + indicesMap[i % 6];
	}

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);
	checkGLErr("glBufferData GL_ARRAY_BUFFER");

	// Position (X, Y, Z)
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, x));
	checkGLErr("glVertexAttribPointer XYZ");
	glEnableVertexAttribArray(0);
	checkGLErr("glEnableVertexAttribArray XYZ");

	// Color (R, G, B, A)
	/*
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, true, sizeof(Vertex), (void*)offsetof(Vertex, r));
	checkGLErr("glVertexAttribPointer RGBA");
	glEnableVertexAttribArray(1);
	checkGLErr("glEnableVertexAttribArray RGBA");
	*/

	// Texture Coordinates (U, V)
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, u));
	checkGLErr("glVertexAttribPointer UV");
	glEnableVertexAttribArray(2);
	checkGLErr("glEnableVertexAttribArray UV");

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	checkGLErr("glBindBuffer GL_ELEMENT_ARRAY_BUFFER (EBO)");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);
	checkGLErr("glBufferData GL_ELEMENT_ARRAY_BUFFER (indices)");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	m_compiled = true;
}

void CubeMesh::unload() const
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void CubeMesh::init(Vec2<int> texSize, Vec2<int> texOffset, bool mirrored)
{
	m_vertices.clear();

	m_texSize = texSize;
	m_texOffset = texOffset;

	m_mirrored = mirrored;
}

void CubeMesh::setRotationPoint(const Vec3<float>& point)
{
	rotationPoint = point;
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

	std::array<float, 3> bottomFrontLeft{ min.x, min.y, min.z };
	std::array<float, 3> bottomFrontRight{ max.x, min.y, min.z };
	std::array<float, 3> topFrontRight{ max.x, max.y, min.z };
	std::array<float, 3> topFrontLeft{ min.x, max.y, min.z };
	std::array<float, 3> bottomBackLeft{ min.x, min.y, max.z };
	std::array<float, 3> bottomBackRight{ max.x, min.y, max.z };
	std::array<float, 3> topBackRight{ max.x, max.y, max.z };
	std::array<float, 3> topBackLeft{ min.x, max.y, max.z };

	std::array<std::array<float, 3>, 4> right{ bottomBackRight, bottomFrontRight, topFrontRight, topBackRight };
	std::array<std::array<float, 3>, 4> left{ bottomFrontLeft, bottomBackLeft, topBackLeft, topFrontLeft };
	std::array<std::array<float, 3>, 4> bottom{ bottomBackRight, bottomBackLeft, bottomFrontLeft, bottomFrontRight };
	std::array<std::array<float, 3>, 4> top{ topFrontRight, topFrontLeft, topBackLeft, topBackRight };
	std::array<std::array<float, 3>, 4> front{ bottomFrontRight, bottomFrontLeft, topFrontLeft, topFrontRight };
	std::array<std::array<float, 3>, 4> back{ bottomBackLeft, bottomBackRight, topBackRight, topBackLeft };

	std::array<float, 4> rightTexCoords{ m_texOffset.x + size.z + size.x, m_texOffset.y + size.z, m_texOffset.x + size.z + size.x + size.z, m_texOffset.y + size.z + size.y };
	std::array<float, 4> leftTexCoords{ m_texOffset.x + 0, m_texOffset.y + size.z, m_texOffset.x + size.z, m_texOffset.y + size.z + size.y };
	std::array<float, 4> bottomTexCoords{ m_texOffset.x + size.z, m_texOffset.y + 0, m_texOffset.x + size.z + size.x, m_texOffset.y + size.z };
	std::array<float, 4> topTexCoords{ m_texOffset.x + size.z + size.x, m_texOffset.y + 0, m_texOffset.x + size.z + size.x + size.x, m_texOffset.y + size.z };
	std::array<float, 4> frontTexCoords{ m_texOffset.x + size.z, m_texOffset.y + size.z, m_texOffset.x + size.z + size.x, m_texOffset.y + size.z + size.y };
	std::array<float, 4> backTexCoords{ m_texOffset.x + size.z + size.x + size.z, m_texOffset.y + size.z, m_texOffset.x + size.z + size.x + size.z + size.x, m_texOffset.y + size.z + size.y };

	// Helper function to add a face with UV coordinates
	auto addFaceUV = [&](
		const std::array<std::array<float, 3>, 4>& vertices,
		const std::array<float, 4>& texCoords)
		{
			float texelW = 0.1f / m_texSize.x;
			float texelH = 0.1f / m_texSize.y;

			m_vertices.push_back(Vertex{
				vertices[0][0], vertices[0][1], vertices[0][2],
				(texCoords[2] / m_texSize.x) - texelW, (texCoords[1] / m_texSize.y) + texelH,
				0, 0, 0
				});
			m_vertices.push_back(Vertex{
				vertices[1][0], vertices[1][1], vertices[1][2],
				(texCoords[0] / m_texSize.x) + texelW, (texCoords[1] / m_texSize.y) + texelH,
				0, 0, 0
				});
			m_vertices.push_back(Vertex{
				vertices[2][0], vertices[2][1], vertices[2][2],
				(texCoords[0] / m_texSize.x) + texelW, (texCoords[3] / m_texSize.y) - texelH,
				0, 0, 0
				});
			m_vertices.push_back(Vertex{
				vertices[3][0], vertices[3][1], vertices[3][2],
				(texCoords[2] / m_texSize.x) - texelW, (texCoords[3] / m_texSize.y) - texelH,
				0, 0, 0
				});
		};

	// Add faces with UV coordinates
	addFaceUV(right, rightTexCoords);
	addFaceUV(left, leftTexCoords);
	addFaceUV(bottom, bottomTexCoords);
	addFaceUV(top, topTexCoords);
	addFaceUV(front, frontTexCoords);
	addFaceUV(back, backTexCoords);

	if (m_mirrored)
	{
		flipFaces();
	}
}

void CubeMesh::render(MatrixStack* matrix, Shader* shader, Texture* texture, float scale)
{
	if (!m_compiled)
	{
		upload(scale);
	}

	matrix->push();

	matrix->translate(rotationPoint.x * scale, rotationPoint.y * scale, rotationPoint.z * scale);

	if (rotationAngle.z != 0.f)
	{
		matrix->rotate(rotationAngle.z * 180.0f / PI, 0.0f, 0.0f, 1.0f);
	}
	if (rotationAngle.y != 0.f)
	{
		matrix->rotate(rotationAngle.y * 180.0f / PI, 0.0f, 1.0f, 0.0f);
	}
	if (rotationAngle.x != 0.f)
	{
		matrix->rotate(rotationAngle.x * 180.0f / PI, 1.0f, 0.0f, 0.0f);
	}

	shader->setMat4("model", false, matrix->top());

	// bind
	glBindVertexArray(m_VAO);
	if (texture != nullptr)
	{
		texture->bind(0);
	}

	// draw
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

	// unbind
	glBindVertexArray(0);
	if (texture != nullptr)
	{
		texture->unbind();
	}

	matrix->pop();
}

void CubeMesh::render(MatrixStack* matrix, Shader* shader, float scale)
{
	render(matrix, shader, nullptr, scale);
}

CubeMesh::~CubeMesh()
{
	unload();
}
