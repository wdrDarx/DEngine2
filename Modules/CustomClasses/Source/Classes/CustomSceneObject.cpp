#include "CustomSceneObject.h"
#include "CustomAppObject.h"
#include "TestAsset.h"

void CustomSceneObject::OnConstruct()
{
	Super::OnConstruct();
}

void CustomSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void CustomSceneObject::testfunc(float test)
{
	LogTemp(STRING(test));
}
