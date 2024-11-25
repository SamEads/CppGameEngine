#include "bufferbuilder.h"
#include <glad/glad.h>
#include <iostream>

void BufferBuilder::bindBuffer(unsigned int target, unsigned int buffer)
{
	glBindBuffer(target, buffer);
}

void BufferBuilder::bufferData(unsigned int target, size_t size, const void* data, unsigned int usage)
{
	glBufferData(target, size, data, usage);
}

BufferBuilder::BufferBuilder()
{
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);
}

BufferBuilder& BufferBuilder::setIndices(const std::vector<unsigned int>& indices)
{
	glBindVertexArray(m_vao);
	bindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	bufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
	return *this;
}

BufferBuilder& BufferBuilder::addAttribute(unsigned int location, int size, unsigned int type, bool normalized, size_t offset)
{
	glBindVertexArray(m_vao);
	bindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glVertexAttribPointer(location, size, type, normalized, m_stride, (void*)offset);
	glEnableVertexAttribArray(location);
	return *this;
}

void BufferBuilder::build()
{
	bindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
