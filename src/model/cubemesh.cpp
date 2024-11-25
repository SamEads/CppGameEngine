#include "cubemesh.h"
#include "../utility/matrixstack.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include <glad/glad.h>
#include "../utility/defines.h"
#include "../graphics/bufferbuilder.h"

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

	m_indices.resize(faceCount * 6);

	// Apply scaling
	for (auto& v : m_vertices)
	{
		v.x *= scale;
		v.y *= scale;
		v.z *= scale;
	}

	// Calculate normals & indices
	calculateNormals();
	calculateIndices();

	BufferBuilder builder;

	builder.setVertexData<Vertex>(m_vertices)
		.setIndices(m_indices)
		.addAttribute(0, 3, GL_FLOAT, false, offsetof(Vertex, x))  // Position
		.addAttribute(1, 2, GL_FLOAT, false, offsetof(Vertex, u))  // UV coords
		.build();

	m_VAO = builder.getVAO();
	m_VBO = builder.getVBO();
	m_EBO = builder.getIBO();

	m_compiled = true;
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
	Vec3<float> max =
	{
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

	std::array<float, 3> bottomFrontLeft { min.x, min.y, min.z };
	std::array<float, 3> bottomFrontRight { max.x, min.y, min.z };
	std::array<float, 3> topFrontRight { max.x, max.y, min.z };
	std::array<float, 3> topFrontLeft { min.x, max.y, min.z };
	std::array<float, 3> bottomBackLeft { min.x, min.y, max.z };
	std::array<float, 3> bottomBackRight { max.x, min.y, max.z };
	std::array<float, 3> topBackRight { max.x, max.y, max.z };
	std::array<float, 3> topBackLeft { min.x, max.y, max.z };

	using FaceVertices = const std::array<std::array<float, 3>, 4>;
	using FaceCoords = const std::array<float, 4>;

	FaceVertices right { bottomBackRight, bottomFrontRight, topFrontRight, topBackRight };
	FaceVertices left { bottomFrontLeft, bottomBackLeft, topBackLeft, topFrontLeft };
	FaceVertices bottom { bottomBackRight, bottomBackLeft, bottomFrontLeft, bottomFrontRight };
	FaceVertices top { topFrontRight, topFrontLeft, topBackLeft, topBackRight };
	FaceVertices front { bottomFrontRight, bottomFrontLeft, topFrontLeft, topFrontRight };
	FaceVertices back { bottomBackLeft, bottomBackRight, topBackRight, topBackLeft };

	// Wrap coordinates in this pattern:
	//	[ TB ] (blank, top, bottom, blank)
	//	[RFLB] (right, front, left, back)

	FaceCoords rightCoords
	{
		m_texOffset.x + size.z + size.x, m_texOffset.y + size.z,
		m_texOffset.x + size.z + size.x + size.z, m_texOffset.y + size.z + size.y
	};
	FaceCoords leftCoords
	{
		m_texOffset.x, m_texOffset.y + size.z,
		m_texOffset.x + size.z, m_texOffset.y + size.z + size.y
	};
	FaceCoords bottomCoords
	{
		m_texOffset.x + size.z, m_texOffset.y,
		m_texOffset.x + size.z + size.x, m_texOffset.y + size.z
	};
	FaceCoords topCoords
	{
		m_texOffset.x + size.z + size.x, m_texOffset.y,
		m_texOffset.x + size.z + size.x + size.x, m_texOffset.y + size.z
	};
	FaceCoords frontCoords
	{
		m_texOffset.x + size.z, m_texOffset.y + size.z,
		m_texOffset.x + size.z + size.x, m_texOffset.y + size.z + size.y
	};
	FaceCoords backCoords
	{
		m_texOffset.x + size.z + size.x + size.z, m_texOffset.y + size.z,
		m_texOffset.x + size.z + size.x + size.z + size.x, m_texOffset.y + size.z + size.y
	};

	auto faceUV = [&](FaceVertices& vertices, FaceCoords& coords)
	{
		float texelW = 0.1f / m_texSize.x;
		float texelH = 0.1f / m_texSize.y;
			
		vertexUV(vertices[0][0], vertices[0][1], vertices[0][2], (coords[2] / m_texSize.x) - texelW, (coords[1] / m_texSize.y) + texelH);
		vertexUV(vertices[1][0], vertices[1][1], vertices[1][2], (coords[0] / m_texSize.x) + texelW, (coords[1] / m_texSize.y) + texelH);
		vertexUV(vertices[2][0], vertices[2][1], vertices[2][2], (coords[0] / m_texSize.x) + texelW, (coords[3] / m_texSize.y) - texelH);
		vertexUV(vertices[3][0], vertices[3][1], vertices[3][2], (coords[2] / m_texSize.x) - texelW, (coords[3] / m_texSize.y) - texelH);
	};

	// Add faces with UV coordinates
	// right
	{
		faceUV(right, rightCoords);
	}
	// left
	{
		faceUV(left, leftCoords);
	}
	// bottom
	{
		faceUV(bottom, bottomCoords);
	}
	// top
	{
		faceUV(top, topCoords);
	}
	// front
	{
		faceUV(front, frontCoords);
	}
	// back
	{
		faceUV(back, backCoords);
	}

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
		matrix->rotate(RAD2DEG(rotationAngle.x), 0.0f, 0.0f, 1.0f);
	}
	if (rotationAngle.y != 0.f)
	{
		matrix->rotate(RAD2DEG(rotationAngle.y), 0.0f, 1.0f, 0.0f);
	}
	if (rotationAngle.x != 0.f)
	{
		matrix->rotate(RAD2DEG(rotationAngle.z), 1.0f, 0.0f, 0.0f);
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

void Mesh::vertexUVNormal(float x, float y, float z, float u, float v, float nx, float ny, float nz)
{
	m_vertices.emplace_back(x, y, z, u, v);
}

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
	//bool showWireframe = false;

	glBindVertexArray(m_VAO);
	texture.bind(0);

	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

	texture.unbind();
	glBindVertexArray(0);
}