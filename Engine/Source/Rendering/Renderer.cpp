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
			m_Scene->DestroyRenderer(this);
		}
	});

	m_Scene->GetApplication()->GetEventDispatcher().Bind(m_ModuleCallback);
}

void Renderer::PrepareFrame()
{
	if(!m_Scene->GetRenderAPI()) return; //no scene render api context to actually render to
}

void Renderer::RenderFrame(Ref<Camera> camera)
{
	if (!m_Scene->GetRenderAPI()) return; //no scene render api context to actually render to
}

void Renderer::ClearFrame()
{
	if (!m_Scene->GetRenderAPI()) return; //no scene render api context to actually render to
}

