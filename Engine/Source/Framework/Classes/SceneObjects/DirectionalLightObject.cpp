#include "DirectionalLightObject.h"
#include "DEngine.h"

void DirectionalLightObject::OnConstruct()
{
	Super::OnConstruct();

	Root = CreateComponent<TransformComponent>(ObjectInitializer(), "Root");
	SetRootComponent(Root);
}

void DirectionalLightObject::OnPostConstruct()
{
	Super::OnPostConstruct();

	m_DirLight = MakeRef<DirectionalLight>();
	GetScene()->GetPipeline()->GetRenderer<MeshRenderer>()->SubmitDirectionalLight(m_DirLight);
}

void DirectionalLightObject::OnDestroy()
{
	Super::OnDestroy();
	GetScene()->GetPipeline()->GetRenderer<MeshRenderer>()->RemoveDirectionalLight(m_DirLight);
}

void DirectionalLightObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	//draw debug 
	Transform rootTrans = GetRootComponent()->GetWorldTransform();
	GetScene()->GetPipeline()->GetRenderer<DebugRenderer>()->DrawDebugCube(rootTrans.pos, rootTrans.rot, {50.f, 50.f, 10.f} , { 0.7f,1.0f,0.7f});
	GetScene()->GetPipeline()->GetRenderer<DebugRenderer>()->DrawDebugLine(rootTrans.pos, World::GetForwardVector(rootTrans.rot) * 100.f + rootTrans.pos, { 0.7f,1.0f,0.7f});

	m_DirLight->Direction = color4(World::GetForwardVector(rootTrans.rot), 1.0);
	m_DirLight->Radiance = color4(Color * Intensity, 1.0f);
	m_DirLight->NearPlane = NearPlane;
	m_DirLight->FarPlane = FarPlane;
	m_DirLight->LightSize = LightSize;
	m_DirLight->MinSoftness = MinSoftness;
	m_DirLight->MaxSoftness = MaxSoftness;
	m_DirLight->CastShadows = CastShadows;
}

