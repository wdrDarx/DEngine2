#pragma once
#include "RenderAPI.h"

struct VertexBufferLayout;
struct VertexBuffer;
struct DENGINE_API VertexArray
{
private:
	uint m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void Addbuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	const VertexBuffer& GetVertexBuffer() const
	{
		ASSERT(m_VertexBuffer);
		return *m_VertexBuffer;
	}

	void Bind() const;
	void Unbind() const;

	const VertexBuffer* m_VertexBuffer;
};

