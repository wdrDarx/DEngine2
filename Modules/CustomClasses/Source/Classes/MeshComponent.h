#pragma once
#include "CustomClasses.h"
#include "CustomSceneObject.h"
#include "DEngine.h"

class CUSTOMCLASSES MeshComponent : public ObjectComponent
{
public:
	OBJECT_CLASS_DEF(MeshComponent, ObjectComponent)
		OBJECT_PROPS_BEGIN()
			PROPDEF(MeshTransform, EditAnywhere)
			PROPDEF(m_MeshAsset, EditAnywhere)
			PROPDEF(m_MaterialAsset, EditAnywhere)
		OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;
	void OnDestroy() override;

public:
	Transform MeshTransform;
	AssetRef<MeshAsset> m_MeshAsset;
	AssetRef<MaterialAsset> m_MaterialAsset;

	UID m_LastMeshAssetID;
	UID m_LastMaterialAssetID;

	Ref<Material> m_Material;
	Ref<Mesh> m_Mesh;
};