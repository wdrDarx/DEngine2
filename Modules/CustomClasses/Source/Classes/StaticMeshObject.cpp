#include "StaticMeshObject.h"

void StaticMeshObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void StaticMeshObject::OnConstruct()
{
	Super::OnConstruct();

	m_Mesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Mesh");
	m_Root = CreateComponent<TransformComponent>(ObjectInitializer::Module(this), "Root");

	m_Mesh->AttachTo(m_Root);
	SetRootComponent(m_Root);
}
