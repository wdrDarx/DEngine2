#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Assets/Classes/TextureAsset.h"
#include "Utils/Task.h"

struct Cubemap
{
	~Cubemap()
	{
		glDeleteTextures(1, &m_CubeMapSlot);
		glDeleteTextures(1, &m_PreFilterMap);
		glDeleteTextures(1, &m_hdrTextureSlot);
		glDeleteTextures(1, &m_brdfMap);
		glDeleteTextures(1, &m_hdrFrameBufferSlot);
		glDeleteTextures(1, &m_IrradianceMap);
	}

	int m_Width;
	int m_Height;
	int m_BPP;
	uint m_CubeMapSlot;
	uint m_hdrTextureSlot;
	uint m_PreFilterMap;
	uint m_brdfMap;
	uint m_hdrFrameBufferSlot;
	uint m_IrradianceMap;
};

class DENGINE_API CubemapRenderer : public Renderer
{
public:
	OBJECT_CLASS_DEF(CubemapRenderer, Renderer)

	void OnConstruct() override;

	void PrepareFrame() override;
	void RenderFrame(Ref<Camera> camera) override;
	void ClearFrame() override;

	Ref<Cubemap> CreateCubemapFromAsset(Ref<TextureAsset> asset);

	void SetActiveCubemap(Ref<Cubemap> ActiveCubemap);
	void ClearActiveCubemap();

	Ref<Cubemap> GetActiveCubemap() const
	{
		return m_ActiveCubemap;
	}

private:
	Ref<Cubemap> m_ActiveCubemap;
};

