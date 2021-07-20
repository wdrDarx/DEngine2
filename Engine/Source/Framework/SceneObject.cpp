#include "SceneObject.h"
#include "DEngine.h"

void SceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void SceneObject::OnConstruct()
{
	Super::OnConstruct();

	//auto destroy object when its module is unloaded
	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		if (event->GetEventType() == ModuleEventType::UNLOADED && event->m_ModuleName == GetAssociatedModuleName())
		{
			GetScene()->DestroySceneObject(this);
		}
	});

	m_Scene->GetApplication()->GetEventDispatcher().Bind(m_ModuleCallback);
}
