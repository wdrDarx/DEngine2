#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"
#include "Classes/CustomSceneObject.h"

void CustomClasses::OnLoad()
{
	REGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, RegistryType::APPOBJECT);
	REGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses, RegistryType::SCENEOBJECT);
}

void CustomClasses::OnUnload()
{
	UNREGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, RegistryType::APPOBJECT);
	UNREGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses, RegistryType::SCENEOBJECT);
}
