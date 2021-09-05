#include "ChildSceneObject.h"
#include "StaticMeshObject.h"

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
		GetScene()->GetPipeline()->GetRenderer<CubemapRenderer>()->SetActiveCubemap(GetScene()->GetPipeline()->GetRenderer<CubemapRenderer>()->CreateCubemapFromAsset(cubemap));
	}

	m_KeyDownEvent.Assign([&](KeyEvent* event)
	{
		if (event->GetKeyCode() == GLFW_KEY_SPACE)
		{
			for(uint i = 0; i < 1000; i++)
			{ 
				Transform RandomTransform;
				RandomTransform.pos.x = Rand().Float() * RandomSpawnRange - RandomSpawnRange * 0.5f;
				RandomTransform.pos.y = Rand().Float() * RandomSpawnRange - RandomSpawnRange * 0.5f;
				RandomTransform.pos.z = Rand().Float() * RandomSpawnRange - RandomSpawnRange * 0.5f;

				RandomTransform.rot.x = Rand().Float() * 180.f;
				RandomTransform.rot.y = Rand().Float() * 180.f;
				RandomTransform.rot.z = Rand().Float() * 180.f;

				Ref<SceneObject> spawned = SceneUtils::SpawnPrefabInScene(PrefabToSpawn, GetScene(), RandomTransform, ObjectInitializer::Module(this));
			}
		}
	});

	GetScene()->GetInputManager().BindOnKeyDown(m_KeyDownEvent);

}

void ChildSceneObject::OnDestroy()
{
	Super::OnDestroy();

	GetScene()->GetPipeline()->GetRenderer<CubemapRenderer>()->ClearActiveCubemap();
}
