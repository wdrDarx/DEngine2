#include "ChildSceneObject.h"
#include "SonicGaming.h"

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

void ChildSceneObject::OnPostConstruct()
{
	Super::OnPostConstruct();

	auto cubemap = GetScene()->GetApplication()->GetAssetManager().LoadAsset(CubemapAssetTest);
	if (cubemap)
	{
		GetScene()->GetRenderer<CubemapRenderer>()->SetActiveCubemap(GetScene()->GetRenderer<CubemapRenderer>()->CreateCubemapFromAsset(cubemap));
	}

	m_KeyDownEvent.Assign([&](KeyEvent* event)
	{
		if (event->GetKeyCode() == GLFW_KEY_SPACE)
		{
			Transform RandomTransform;
			RandomTransform.pos.x = Rand::Float() * RandomSpawnRange;
			RandomTransform.pos.y = Rand::Float() * RandomSpawnRange;
			RandomTransform.pos.z = Rand::Float() * RandomSpawnRange;

			RandomTransform.rot.x = Rand::Float() * RandomSpawnRange;
			RandomTransform.rot.y = Rand::Float() * RandomSpawnRange;
			RandomTransform.rot.z = Rand::Float() * RandomSpawnRange;

			Ref<SonicGaming> spawned = Cast<SonicGaming>(SceneUitls::SpawnPrefabInScene(PrefabToSpawn, GetScene(), Transform(), ObjectInitializer::Module(this)));
			spawned->m_MeshComponent->MeshTransform = RandomTransform;
		}
	});

	GetScene()->GetInputManager().BindOnKeyDown(m_KeyDownEvent);

}

void ChildSceneObject::OnDestroy()
{
	Super::OnDestroy();

	GetScene()->GetRenderer<CubemapRenderer>()->ClearActiveCubemap();
}
