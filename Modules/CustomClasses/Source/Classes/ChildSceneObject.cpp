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
