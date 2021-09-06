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
			PROPDEF(AutoFollowCamera, EditAnywhere);
			PROPDEF(SourceLength, EditAnywhere);
			PROPDEF(ShadowMapScale, EditAnywhere);
			PROPDEF(NearPlane, EditAnywhere);
			PROPDEF(FarPlane, EditAnywhere);
	OBJECT_PROPS_END()

	void OnConstruct() override;
	void OnPostConstruct() override;
	void OnDestroy() override;
	void OnUpdate(const Tick& tick) override;

	float Intensity = 1.f;
	color3 Color = {1,1,1};
	float SourceLength = 1000.f;
	float ShadowMapScale = 1000.f;
	float NearPlane = 10.f;
	float FarPlane = 10000.f;

	bool AutoFollowCamera = false;

private:
	Ref<DirectionalLight> m_DirLight = nullptr;
	Ref<TransformComponent> Root = nullptr;
};

