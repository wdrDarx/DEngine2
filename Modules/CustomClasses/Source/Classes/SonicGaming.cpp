#include "SonicGaming.h"

void SonicGaming::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	auto moveLamba = [&](Transform& trans)
	{
		if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_W))
			trans.pos.y += Speed * tick.DeltaTime;

		if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_A))
			trans.pos.x -= Speed * tick.DeltaTime;

		if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_S))
			trans.pos.y -= Speed * tick.DeltaTime;

		if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_D))
			trans.pos.x += Speed * tick.DeltaTime;
	};
}

void SonicGaming::OnConstruct()
{
	Super::OnConstruct();

	m_RootMesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "RootMesh");
	m_SubMesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "SubMesh");
	m_SubMesh->AttachTo(m_RootMesh);
	SetRootComponent(m_RootMesh);
}
