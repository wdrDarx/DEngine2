#include "SkyLightObject.h"
#include "DEngine.h"

void SkyLightObject::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);
	if (auto renderer = GetScene()->GetPipeline()->GetRenderer<MeshRenderer>())
	{
		renderer->GetSettingsMutable().AmbientLightMultiplier = SkyIntensity;
	}
}

void SkyLightObject::OnConstruct()
{
	Super::OnConstruct();
}

void SkyLightObject::OnPostConstruct()
{
	Super::OnPostConstruct();

	auto cubemap = GetScene()->GetApplication()->GetAssetManager().LoadAsset(SkyBoxCubemap);
	if (cubemap)
	{
		GetScene()->GetPipeline()->GetRenderer<CubemapRenderer>()->SetActiveCubemap(GetScene()->GetPipeline()->GetRenderer<CubemapRenderer>()->CreateCubemapFromAsset(cubemap));
	}
}

void SkyLightObject::OnDestroy()
{
	Super::OnDestroy();
	GetScene()->GetPipeline()->GetRenderer<CubemapRenderer>()->ClearActiveCubemap();
}