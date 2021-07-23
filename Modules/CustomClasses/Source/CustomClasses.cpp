#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"
#include "Classes/CustomSceneObject.h"
#include "Classes/ChildSceneObject.h"

void CustomClasses::OnLoad()
{
	REGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses);
	REGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses);
	REGISTER(GetApplication()->GetRegistry(), ChildSceneObject, CustomClasses);
}

void CustomClasses::OnUnload()
{
	UNREGISTER(GetApplication()->GetRegistry(), CustomAppObject, CustomClasses);
	UNREGISTER(GetApplication()->GetRegistry(), CustomSceneObject, CustomClasses);
	UNREGISTER(GetApplication()->GetRegistry(), ChildSceneObject, CustomClasses);
}
