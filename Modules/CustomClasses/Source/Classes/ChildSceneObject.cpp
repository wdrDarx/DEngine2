#include "ChildSceneObject.h"
#include "CustomSceneObject.h"

void ChildSceneObject::OnConstruct()
{
	Super::OnConstruct();
	//LogTemp("child obj");
	auto staticClass = OBJECT_STATIC_CLASS(ChildSceneObject);

	//auto& props = staticClass.GetDefaultProperties();
}
