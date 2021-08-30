#pragma once
#include "DEngine.h"
#include "CameraComponent.h"
#include "StaticMeshComponent.h"
#include "CustomClasses.h"

class CUSTOMCLASSES PlayerController : public SceneObject
{
public:	
	OBJECT_CLASS_DEF(PlayerController, SceneObject)
	OBJECT_PROPS_BEGIN()
		PROPDEF(MovementSpeed, EditAnywhere);
		PROPDEF(RoatationLerpSpeed, EditAnywhere);
		PROPDEF(FlipForwardVector, EditAnywhere);
		PROPDEF(BulletPrefab, EditAnywhere);
	OBJECT_PROPS_END()

	void OnConstruct() override;
	void OnUpdate(const Tick& tick) override;
	void OnBeginPlay() override;

public:
	float MovementSpeed = 100.f;
	float RoatationLerpSpeed = 0.5f;
	bool FlipForwardVector = true;

	Ref<TransformComponent> root;
	Ref<CameraComponent> camera;
	Ref<StaticMeshComponent> mesh;

	AssetRef<PrefabAsset> BulletPrefab;

	vec3d TargetRotation;

	Callback<MouseEvent> mouseEvent;
};

