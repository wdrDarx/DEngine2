#include "QuadComponent.h"
#include "QuadRenderer.h"

void QuadComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	GetOwner()->GetScene()->GetRenderer<QuadRenderer>()->DrawQuad3D({ QuadSize.x, QuadSize.y,1 }, QuadTransform, QuadColor);
}

void QuadComponent::OnConstruct()
{
	Super::OnConstruct();
}
