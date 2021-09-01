#include "PhysicsWorld.h"
#include "PhysicsCore.h"

void PhysicsWorld::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
}

void PhysicsWorld::OnConstruct()
{
	Super::OnConstruct();

	m_PhysicsLayerManager = MakeRef<PhysicsLayerManager>();
	m_API = MakeRef<PhysXAPI>();
	m_CookingFactory = MakeRef<CookingFactory>(GetPhysicsAPI().get());

	m_PhysicsLayerManager->AddLayer("Default");
}

void PhysicsWorld::OnDestroy()
{
	Super::OnDestroy();
}

void PhysicsWorld::OnBeginPlay()
{

}

void PhysicsWorld::OnEndPlay()
{

}
