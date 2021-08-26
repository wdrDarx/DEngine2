#include "QuadComponent.h"

void QuadComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	auto asset = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(QuadTexture);

	for(uint i = 0; i < QuadAmount; i++)
	{
		Transform final = BaseQuadTransform;
		final.pos.x += i * QuadSize.x + 10.f;
		GetOwner()->GetScene()->GetPipeline()->GetRenderer<QuadRenderer>()->DrawQuad3D({QuadSize.x, QuadSize.y}, final, QuadColor, asset ? asset->GetTexture() : nullptr);
	}
}

void QuadComponent::OnConstruct()
{
	Super::OnConstruct();

}
