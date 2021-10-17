#pragma once
#include "DEngine.h"
#include "CameraComponent.h"
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
	float MovementSpeed = 2000.f;
	float RoatationLerpSpeed = 20.f;
	bool FlipForwardVector = true;

	Ref<BoxColliderComponent> collider = nullptr;
	Ref<TransformComponent> BulletSpawn = nullptr;
	Ref<TransformComponent> root = nullptr;
	Ref<CameraComponent> camera = nullptr;
	Ref<StaticMeshComponent> mesh = nullptr;

	AssetRef<PrefabAsset> BulletPrefab;

	vec3d TargetRotation;
	vec3d MovementVecAdd = {0,0,0};

	Callback<MouseEvent> mouseEvent;
};

