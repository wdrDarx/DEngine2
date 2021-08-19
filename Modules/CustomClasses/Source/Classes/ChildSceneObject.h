#pragma once
#include "CustomClasses.h"
#include "DEngine.h"
#include "QuadComponent.h"

class ChildSceneObject : public SceneObject
{
public:
	OBJECT_CLASS_DEF(ChildSceneObject, SceneObject)
		OBJECT_PROPS_BEGIN()
			PROPDEF(test, EditAnywhere);
			PROPDEF(CubemapAssetTest, EditAnywhere);
			PROPDEF(Speed, EditAnywhere);
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;
	void OnPostConstruct() override;
	void OnDestroy() override;

public:
	Ref<QuadComponent> m_QuadComponent;
	AssetRef<TextureAsset> CubemapAssetTest;

	float test = 12.f;
	float Speed = 30.f;
};

