#pragma once
#include "RenderAPI.h"

struct DENGINE_API UniformStorageBuffer
{
private:
	uint m_RendererID = 0;
	uint m_LastSize = 0;
	uint m_Bindingindex = 0;
public:
	UniformStorageBuffer(uint Bindingindex);
	UniformStorageBuffer(uint Bindingindex, const void* Data, uint size);
	UniformStorageBuffer(uint Bindingindex, uint size);

	~UniformStorageBuffer();

	void SetData(const void* Data, uint size);

	void Bind() const;
	void Unbind() const;
};
