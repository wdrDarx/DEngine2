#include "ChildSceneObject.h"
#include "CustomSceneObject.h"
#include "QuadRenderer.h"

void ChildSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	GetScene()->GetRenderer<QuadRenderer>()->DrawQuad2D(QuadPos, {50, 50 }, {1, 0,0,1});
}

void ChildSceneObject::OnConstruct()
{
	Super::OnConstruct();
// 	auto staticClass = OBJECT_STATIC_CLASS(ChildSceneObject);
// 	auto& props = staticClass.GetDefaultProperties();
// 
// 
// 	auto staticClassStruct = STRUCT_STATIC_CLASS(TestStruct2);
// 	auto& prop2s = staticClassStruct.GetDefaultProperties();
// 
// 	auto props3 = staticClassStruct.GenerateStaticProperties(GetScene()->GetApplication());
}
