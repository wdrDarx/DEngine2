#include "ChildSceneObject.h"


void ChildSceneObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	m_QuadComponent->RotationSpeed = Speed;
}

void ChildSceneObject::OnConstruct()
{
	Super::OnConstruct();
	m_QuadComponent = CreateComponent<QuadComponent>(ObjectInitializer::Module(this), "MyQuadComponent");	
}

void ChildSceneObject::OnPostConstruct()
{
	Super::OnPostConstruct();

	auto cubemap = GetScene()->GetApplication()->GetAssetManager().LoadAsset(CubemapAssetTest);
	if (cubemap)
	{
		GetScene()->GetRenderer<CubemapRenderer>()->SetActiveCubemap(GetScene()->GetRenderer<CubemapRenderer>()->CreateCubemapFromAsset(cubemap));
	}

}

void ChildSceneObject::OnDestroy()
{
	Super::OnDestroy();

	GetScene()->GetRenderer<CubemapRenderer>()->ClearActiveCubemap();
}
