#pragma once

#include <vector>

class BufferBuilder
{
private:
	size_t m_stride = 0;
	
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;

	void bindBuffer(unsigned int target, unsigned int buffer);

	void bufferData(unsigned int target, size_t size, const void* data, unsigned int usage);

public:

	unsigned int getVAO() const
	{
		return m_vao;
	}

	unsigned int getVBO() const
	{
		return m_vbo;
	}

	unsigned int getIBO() const
	{
		return m_ibo;
	}

	BufferBuilder();

	template<typename T>
	BufferBuilder(const std::vector<T>& data) : BufferBuilder()
	{
		setVertexData(data);
	}

	template<typename T>
	BufferBuilder& setVertexData(const std::vector<T>& data)
	{
		m_stride = sizeof(T);
		bindBuffer(0x8892, m_vbo);
		bufferData(0x8892 /*GL_ARRAY_BUFFER*/, data.size() * sizeof(T), data.data(), /*GL_STATIC_DRAW*/ 0x88E4);
		return *this;
	}

	BufferBuilder& setIndices(const std::vector<unsigned int>& indices);

	BufferBuilder& addAttribute(unsigned int location, int size, unsigned int type, bool normalized, size_t offset);

	void build();
};