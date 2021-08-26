#include "Renderer.h"
#include "DEngine.h"

void Renderer::OnConstruct()
{
	Super::OnConstruct();

	//auto destroy renderer if its outer module is unloaded
	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		if (event->GetEventType() == ModuleEventType::UNLOADED && event->m_ModuleName == GetAssociatedModuleName())
		{
			GetPipeline()->DestroyRenderer(this);
		}
	});

	GetPipeline()->GetScene()->GetApplication()->GetEventDispatcher().Bind(m_ModuleCallback);
}

void Renderer::PrepareFrame()
{
	if(!GetPipeline()->GetRenderAPI()) return; //no scene render api context to actually render to
}

void Renderer::RenderFrame(Ref<Camera> camera)
{
	if (!GetPipeline()->GetRenderAPI()) return; //no scene render api context to actually render to
}

void Renderer::ClearFrame()
{
	if (!GetPipeline()->GetRenderAPI()) return; //no scene render api context to actually render to
}

