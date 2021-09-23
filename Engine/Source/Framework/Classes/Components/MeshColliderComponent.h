#pragma once
#include "ColliderComponent.h"
#include "Assets/AssetRef.h"
#include "Assets/Classes/MeshAsset.h"
#include "Rendering/Mesh.h"

class DENGINE_API MeshColliderComponent : public ColliderComponent
{
public:
	OBJECT_CLASS_DEF(MeshColliderComponent, ColliderComponent);
	OBJECT_PROPS_BEGIN()
		PROPS_CATEGORY(MeshCollider)
			PROPDEF(ColliderMesh, EditAnywhere);
			PROPDEF(Convex, EditAnywhere);
	OBJECT_PROPS_END()

	void OnBeginPlay() override;
	void OnEndPlay() override;
	void OnUpdate(const Tick& tick) override;

	Ref<ColliderShape> CreateColliderShape() override;
	AssetRef<MeshAsset> ColliderMesh;
	Ref<Mesh> m_LoadedMesh = nullptr;
	bool Convex = true;
};

