#pragma once
#include "Rendering/Pipeline.h"
#include "Rendering/FrameBuffer.h"

struct DeferredFrame;
class DENGINE_API DefaultPipeline : public Pipeline
{
	public:
		OBJECT_CLASS_DEF(DefaultPipeline, Pipeline)

		void OnConstruct() override;
		void PrepareFrame() override;
		void RenderFrame(Ref<Camera> camera) override;

		//auto creates new render target if index exceeds the array size of m_BaseFrames
		Ref<FrameBuffer> GetRenderTargetFromIndex(uint index);

		//this is where the scene is rendered to before post processing (one frame buffer per render target e.g a viewport)
		std::vector<Ref<FrameBuffer>> m_BaseFrames;
		FrameBufferSpec m_BaseFrameSpec;
		uint m_CurrentRenderTargetIndex = 0;
		uint m_LastNumberOfRenderTargets = 0;

		//used with deferred rendering
		Ref<DeferredFrame> m_DeferredFrame = nullptr;

		//whether to use post processing or no 
		bool m_PostProcessing = true;
		bool m_DefferedRendering = true;

		//resolution scalar
		float m_RenderScale = 1.0; 

		//5 = 16 MSAA samples, 4 = 8 MSAA Samples, 3 = 4 MSAA Samples, 2 = 2 MSAA Samples, 1 = FXAA, 0 = Off
		uint m_AntiAliasingQuality = 5; 
};