#pragma once
#include "DEngine.h"
#include "CameraComponent.h"
#include "StaticMeshComponent.h"
#include "CustomClasses.h"

class CUSTOMCLASSES FirstPersonPawn : public SceneObject
{
public:
	OBJECT_CLASS_DEF(FirstPersonPawn, SceneObject)
	OBJECT_PROPS_BEGIN()
		PROPDEF(MovementSpeed, EditAnywhere);
		PROPDEF(RotationSpeed, EditAnywhere);
		PROPDEF(JumpHeight, EditAnywhere);
		PROPDEF(BulletPrefab, EditAnywhere);
		PROPDEF(BulletForce, EditAnywhere);
		PROPDEF(BlinkSpeed, EditAnywhere);
	OBJECT_PROPS_END()

	void OnConstruct() override;
	void OnUpdate(const Tick& tick) override;
	void OnBeginPlay() override;

	vec2d GetSmoothCameraRot(float deltaTime, float Springiness, const vec2d& MouseDelta);

public:
	float MovementSpeed = 800.f;
	float RotationSpeed = 50.f;
	float JumpHeight = 1500.f;
	float BulletForce = 1500.f;
	float BlinkSpeed = 1000.f;

	Ref<BoxColliderComponent> collider = nullptr;
	Ref<TransformComponent> BulletSpawn = nullptr;
	Ref<CameraComponent> camera = nullptr;
	Ref<StaticMeshComponent> mesh = nullptr;
	Ref<StaticMeshComponent> gun = nullptr;
	Callback<PhysicsActorEvent> m_PhysicsCallback;
	bool m_Grounded = false;

	AssetRef<PrefabAsset> BulletPrefab;

	Callback<MouseEvent> MouseMoveCallback;
	Callback<MouseEvent> MouseDownCallback;
	Callback<KeyEvent> keyEvent;

	vec2d m_MouseMoveVector;
	vec3d m_ControlRotation;
	vec3d m_TargetRotation;
	vec3d m_CameraOffset;
};
