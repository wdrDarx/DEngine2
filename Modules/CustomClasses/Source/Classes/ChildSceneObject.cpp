#include "ChildSceneObject.h"
#include "CustomSceneObject.h"

void ChildSceneObject::OnConstruct()
{
	Super::OnConstruct();
	auto staticClass = OBJECT_STATIC_CLASS(ChildSceneObject);
	auto& props = staticClass.GetDefaultProperties();
}
