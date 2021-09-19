#pragma once
#include "Framework/SceneObject.h"
#include "Assets/Classes/TextureAsset.h"
#include "Rendering/Classes/Renderers/MeshRenderer.h"

class DENGINE_API SkyLightObject : public SceneObject
{
public:
	OBJECT_CLASS_DEF(SkyLightObject, SceneObject)
	OBJECT_PROPS_BEGIN()
		PROPDEF(SkyBoxCubemap, EditAnywhere);
		PROPDEF(SkyIntensity, EditAnywhere);
	OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;
	void OnPostConstruct() override;
	void OnDestroy() override;

public:
	AssetRef<TextureAsset> SkyBoxCubemap;
	float SkyIntensity = 0.5f;
};

