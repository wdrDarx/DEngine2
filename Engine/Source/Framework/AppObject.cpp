#include "AppObject.h"
#include "Application.h"

void AppObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void AppObject::OnConstruct()
{
	Super::OnConstruct();

	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		if(event->GetEventType() == ModuleEventType::UNLOADED && event->m_ModuleName == GetAssociatedModuleName())
		{ 
			GetApplication()->DestroyAppObject(this);
		
		}
	});

	m_Application->GetEventDispatcher().Bind(m_ModuleCallback);
}
