#include "ObjectComponent.h"
#include "DEngine.h"

void ObjectComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void ObjectComponent::OnConstruct()
{
	Super::OnConstruct();

	//auto destroy object when its module is unloaded
	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		//prevent auto delete if scene object was deleted prior
		if(!m_ModuleCallback.m_OnTrigger) return;
		if (event->GetEventType() == ModuleEventType::UNLOADED && event->m_ModuleName == GetAssociatedModuleName())
		{
			GetOwner()->DestroyComponent(this);
		}
	});

	GetOwner()->GetScene()->GetApplication()->GetEventDispatcher().Bind(m_ModuleCallback);
}
