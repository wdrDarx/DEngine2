#include "StaticMeshObject.h"

void StaticMeshObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void StaticMeshObject::OnConstruct()
{
	Super::OnConstruct();

	m_Mesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Mesh");

	SetRootComponent(m_Mesh);
}
