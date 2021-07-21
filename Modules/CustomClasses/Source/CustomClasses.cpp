#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"
#include "Classes/CustomSceneObject.h"
#include "Classes/ChildSceneObject.h"

void CustomClasses::OnLoad()
{
	REGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, RegistryType::APPOBJECT);
	REGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses, RegistryType::SCENEOBJECT);
	REGISTER(GetApplication()->GetRegistry(), ChildSceneObject, CustomClasses, RegistryType::SCENEOBJECT);
}

void CustomClasses::OnUnload()
{
	UNREGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, RegistryType::APPOBJECT);
	UNREGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses, RegistryType::SCENEOBJECT);
	UNREGISTER(GetApplication()->GetRegistry(), ChildSceneObject, CustomClasses, RegistryType::SCENEOBJECT);
}
