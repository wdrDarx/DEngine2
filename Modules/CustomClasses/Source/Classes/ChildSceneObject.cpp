#include "ChildSceneObject.h"
#include "CustomSceneObject.h"

void ChildSceneObject::OnConstruct()
{
	Super::OnConstruct();
	auto staticClass = OBJECT_STATIC_CLASS(ChildSceneObject);
	auto& props = staticClass.GetDefaultProperties();


	auto staticClassStruct = STRUCT_STATIC_CLASS(TestStruct2);
	auto& prop2s = staticClassStruct.GetDefaultProperties();

	auto props3 = staticClassStruct.GenerateStaticProperties(GetScene()->GetApplication());
}
