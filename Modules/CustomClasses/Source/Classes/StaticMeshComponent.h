#pragma once
#include "CustomClasses.h"
#include "CustomSceneObject.h"
#include "DEngine.h"

class CUSTOMCLASSES StaticMeshComponent : public MeshColliderComponent
{
public:
	OBJECT_CLASS_DEF(StaticMeshComponent, MeshColliderComponent)
		OBJECT_PROPS_BEGIN()
		PROPS_CATEGORY(Static Mesh);
			PROPDEF(m_MeshAsset, EditAnywhere)
			PROPDEF(m_MaterialAsset, EditAnywhere)
			PROPDEF(CastShadow, EditAnywhere)
			PROPDEF(Visible, EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;
	void OnDestroy() override;

public:
	AssetRef<MeshAsset> m_MeshAsset;
	AssetRef<MaterialAsset> m_MaterialAsset;

	Callback<AssetEvent> m_AssetCallback;
	Ref<StaticMesh> m_StaticMesh;
	bool CastShadow = true;
	bool Visible = true;
};