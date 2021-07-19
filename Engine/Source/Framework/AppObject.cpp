#include "AppObject.h"
#include "Application.h"

void AppObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	//MarkDelete();
}

void AppObject::OnConstruct()
{
	Super::OnConstruct();

	m_ModuleCallback.Assign([&](ModuleEvent* event)
	{
		if(event->GetEventType() == ModuleEventType::UNLOADED)
		{ 
			//if (event->m_ModuleName == GetAssignedModuleName())
			{
				GetApplication()->DestroyAppObject(this);

			}
		}
	});

	m_Application->GetEventDispatcher().Bind(m_ModuleCallback);
}
