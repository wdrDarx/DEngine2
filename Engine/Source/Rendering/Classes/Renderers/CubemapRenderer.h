#pragma once
#include "Rendering/RenderCore.h"
#include "Rendering/Renderer.h"
#include "Assets/Classes/TextureAsset.h"
#include "Utils/Task.h"

struct Cubemap
{
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

	Cubemap CreateCubemapFromAsset(Ref<TextureAsset> asset);

	void SetActiveCubemap(const Cubemap& ActiveCubemap);
	void ClearActiveCubemap();

private:
	std::optional<Cubemap> m_ActiveCubemap;
};

