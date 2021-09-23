#pragma once
#include "Framework/SceneObject.h"
#include "Rendering/Classes/Renderers/MeshRenderer.h"

class DENGINE_API DirectionalLightObject : public SceneObject
{
public:	
	OBJECT_CLASS_DEF(DirectionalLightObject, SceneObject);
	OBJECT_PROPS_BEGIN()
		PROPS_CATEGORY(Light)
			PROPDEF(Color, EditAnywhere);
			PROPDEF(Intensity, EditAnywhere);
		PROPS_CATEGORY(Details)
			PROPDEF(CastShadows, EditAnywhere);
			PROPDEF(LightSize, EditAnywhere);		
			PROPDEF(NearPlane, EditAnywhere);
			PROPDEF(FarPlane, EditAnywhere);
			PROPDEF(MinSoftness, EditAnywhere);
			PROPDEF(MaxSoftness, EditAnywhere);
	OBJECT_PROPS_END()

	void OnConstruct() override;
	void OnPostConstruct() override;
	void OnDestroy() override;
	void OnUpdate(const Tick& tick) override;

	bool CastShadows = true;
	float Intensity = 1.f;
	color3 Color = {1,1,1};
	float LightSize = 1000.f;
	float NearPlane = 0.01f;
	float FarPlane = 10000.f;

	float MinSoftness = 0.0008f;
	float MaxSoftness = 0.02f;

private:
	Ref<DirectionalLight> m_DirLight = nullptr;
	Ref<TransformComponent> Root = nullptr;
};

