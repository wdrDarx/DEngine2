#include "CustomClasses.h"
#include "DEngine.h"

#include "Classes/CustomAppObject.h"
#include "Classes/CustomSceneObject.h"
#include "Classes/ChildSceneObject.h"

void CustomClasses::OnLoad()
{
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);

	REGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct);
	REGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct2);
}

void CustomClasses::OnUnload()
{
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);

	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct);
	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct2);
}
