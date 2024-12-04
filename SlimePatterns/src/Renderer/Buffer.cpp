#include "Buffer.h"

#include <GL/glew.h>

Buffer::Buffer(uint64_t size, uint32_t Binding)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, Binding, m_RendererID);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void Buffer::SetData(const void* data, uint64_t size, uint32_t offset)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
