#include "QuadComponent.h"
#include "QuadRenderer.h"

void QuadComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if(auto asset = GetOwner()->GetScene()->GetApplication()->GetAssetManager().LoadAsset(MessageAsset))
		LogWarning(asset->m_Message);

	GetOwner()->GetScene()->GetRenderer<QuadRenderer>()->DrawQuad3D({ QuadSize.x, QuadSize.y,1 }, QuadTransform, QuadColor);
}

void QuadComponent::OnConstruct()
{
	Super::OnConstruct();
}
