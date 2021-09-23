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
	m_CookingFactory = MakeRef<CookingFactory>(this);

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

Ref<PhysicsScene> PhysicsWorld::CreatePhysicsScene(const PhysicsSettings* CustomSettings /*= nullptr*/)
{
	Ref<PhysicsScene> scene = MakeRef<PhysicsScene>(this, CustomSettings ? *CustomSettings : GetPhysicsSettings());
	m_PhysicsScenes.push_back(scene);
	return scene;
}
