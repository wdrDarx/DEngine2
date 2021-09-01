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
		GetOwner()->GetScene()->GetPipeline()->GetRenderer<DebugRenderer>()->DrawDebugLine(final.pos, World::GetForwardVector(final.rot) * 100.f + final.pos, {1.0f, 0,0});
	}
}

void QuadComponent::OnConstruct()
{
	Super::OnConstruct();

}
