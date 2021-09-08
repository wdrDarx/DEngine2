#include "CameraComponent.h"

void CameraComponent::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if(tick.GetTickGroup() != TickGroup::GAME || !m_Camera) return;

	if (m_LastProjectionType != Projection.Value())
	{
		m_LastProjectionType = Projection.Value();
		m_Camera->SetProjectionType(Projection.Value());
	}

	m_Camera->SetTransform(GetWorldTransform());
	m_Camera->SetFOV(FOV);
}

void CameraComponent::OnBeginPlay()
{
	Super::OnBeginPlay();

	m_Camera = MakeRef<Camera>(GetOwner()->GetScene()->GetPipeline()->GetRenderAPI());
	GetOwner()->GetScene()->SetActiveCamera(m_Camera);
}
