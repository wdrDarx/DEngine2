#include "CustomSceneObject.h"
#include "CustomAppObject.h"

void CustomSceneObject::OnConstruct()
{
	Super::OnConstruct();

	auto staticClass = STRUCT_STATIC_CLASS(TestStruct2);
	auto props = staticClass.GetDefaultProperties();
}

void CustomSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void CustomSceneObject::testfunc(float test)
{
	LogTemp(STRING(test));
}
