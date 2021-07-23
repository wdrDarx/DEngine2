#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"
#include "Classes/CustomSceneObject.h"
#include "Classes/ChildSceneObject.h"

void CustomClasses::OnLoad()
{
	REGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, ObjectClassType::APPOBJECT);
	REGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses, ObjectClassType::SCENEOBJECT);
	REGISTER(GetApplication()->GetRegistry(), ChildSceneObject, CustomClasses, ObjectClassType::SCENEOBJECT);
}

void CustomClasses::OnUnload()
{
	UNREGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses, ObjectClassType::APPOBJECT);
	UNREGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses, ObjectClassType::SCENEOBJECT);
	UNREGISTER(GetApplication()->GetRegistry(), ChildSceneObject, CustomClasses, ObjectClassType::SCENEOBJECT);
}
