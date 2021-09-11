#include "PhysicsWorld.h"
#include "PhysicsCore.h"
#include "json/json.h"

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

	//add layers
	CollisionLayerEnum temp;
	auto map = temp.GetEnumMap();
	for (auto& val : map)
	{
		m_PhysicsLayerManager->AddLayer(val.first);
	}
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
