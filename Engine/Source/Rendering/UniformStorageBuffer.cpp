#include "UniformStorageBuffer.h"

UniformStorageBuffer::UniformStorageBuffer(uint Bindingindex) : m_Bindingindex(Bindingindex)
{
	glGenBuffers(1, &m_RendererID);
}

UniformStorageBuffer::UniformStorageBuffer(uint Bindingindex, const void* Data, uint size) : m_Bindingindex(Bindingindex)
{
	glGenBuffers(1, &m_RendererID);
	SetData(Data, size);
}

UniformStorageBuffer::UniformStorageBuffer(uint Bindingindex, uint size) : m_Bindingindex(Bindingindex)
{
	glGenBuffers(1, &m_RendererID);
	SetData(nullptr, size);
}

UniformStorageBuffer::~UniformStorageBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void UniformStorageBuffer::SetData(const void* Data, uint size)
{
	Bind();
	if(size == m_LastSize)
		glBufferSubData(GL_UNIFORM_BUFFER, 0, size, Data);
	else
		glBufferData(GL_UNIFORM_BUFFER, size, Data, GL_DYNAMIC_DRAW);

	m_LastSize = size;
}


void UniformStorageBuffer::Bind() const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, m_Bindingindex, m_RendererID);
}

void UniformStorageBuffer::Unbind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
