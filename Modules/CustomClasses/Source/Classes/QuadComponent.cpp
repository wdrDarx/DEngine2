#include "QuadComponent.h"
#include "QuadRenderer.h"

void QuadComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if(auto asset = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(QuadTexture))
		GetOwner()->GetScene()->GetRenderer<QuadRenderer>()->DrawQuad3D({ QuadSize.x, QuadSize.y,1 }, QuadTransform, QuadColor, asset->GetTexture());
}

void QuadComponent::OnConstruct()
{
	Super::OnConstruct();
}
