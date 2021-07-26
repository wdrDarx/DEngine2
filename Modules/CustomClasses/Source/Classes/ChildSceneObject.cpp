#include "ChildSceneObject.h"
#include "CustomSceneObject.h"
#include "QuadRenderer.h"

void ChildSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	for (uint i = 0; i < 1000; i++)
	{
		Transform finalTrans = QuadTransform;
		finalTrans.pos.y += i * 110.f;
		GetScene()->GetRenderer<QuadRenderer>()->DrawQuad3D({ 100,100,1 }, finalTrans, QuadColor);
	}
		
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
