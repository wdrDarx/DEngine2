#include "CustomSceneObject.h"
#include "CustomAppObject.h"
#include "TestAsset.h"

void CustomSceneObject::OnConstruct()
{
	Super::OnConstruct();

//	AssetRef<TestAsset> ref(Paths::GetModulesDirectory() + "\\Message");
	Ref<TestAsset> assetref = MakeRef<TestAsset>("hello this is test asset");
	GetScene()->GetApplication()->GetAssetManager().SaveAsset(assetref, Paths::GetModulesDirectory() + "\\Message.TestAsset");
}

void CustomSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void CustomSceneObject::testfunc(float test)
{
	LogTemp(STRING(test));
}
