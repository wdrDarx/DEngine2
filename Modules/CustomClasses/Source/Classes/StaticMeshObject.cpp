#include "StaticMeshObject.h"

void StaticMeshObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void StaticMeshObject::OnConstruct()
{
	Super::OnConstruct();

	m_Mesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Mesh");
	m_Collider = CreateComponent<BoxColliderComponent>(ObjectInitializer::Module(this), "Collider");

	m_Mesh->AttachTo(m_Collider);
	SetRootComponent(m_Collider);
}
