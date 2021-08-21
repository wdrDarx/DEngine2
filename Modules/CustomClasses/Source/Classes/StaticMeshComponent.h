#pragma once
#include "CustomClasses.h"
#include "CustomSceneObject.h"
#include "DEngine.h"

class CUSTOMCLASSES StaticMeshComponent : public TransformComponent
{
public:
	OBJECT_CLASS_DEF(StaticMeshComponent, TransformComponent)
		OBJECT_PROPS_BEGIN()
		PROPDEF(m_MeshAsset, EditAnywhere)
		PROPDEF(m_MaterialAsset, EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;
	void OnDestroy() override;

public:
	AssetRef<MeshAsset> m_MeshAsset;
	AssetRef<MaterialAsset> m_MaterialAsset;

	Ref<StaticMesh> m_StaticMesh;
};