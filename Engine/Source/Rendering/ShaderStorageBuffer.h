#pragma once
#include "RenderAPI.h"

struct DENGINE_API ShaderStorageBuffer
{
private:
	uint m_RendererID = 0;
	uint m_LastSize = 0;
	uint m_Bindingindex = 0;
public:
	ShaderStorageBuffer(uint Bindingindex);
	ShaderStorageBuffer(uint Bindingindex, const void* Data, uint size);
	ShaderStorageBuffer(uint Bindingindex, uint size);

	~ShaderStorageBuffer();

	void SetData(const void* Data, uint size);

	//make sure the data is allocated before calling
	void SetDataOffset(uint offset, const void* Data, uint size);

	//will allocate if not already the right size only
	void Allocate(uint size);

	void Bind() const;
	void Unbind() const;
};
