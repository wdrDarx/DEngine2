#include "PlayerController.h"
#include "Bullet.h"

void PlayerController::OnConstruct()
{
	Super::OnConstruct();

	root = CreateComponent<TransformComponent>(ObjectInitializer::Module(this), "Root");
	mesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Mesh");
	camera = CreateComponent<CameraComponent>(ObjectInitializer::Module(this), "Camera");

	SetRootComponent(root);
	mesh->AttachTo(root);
	camera->AttachTo(root);
}

void PlayerController::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if(tick.GetTickGroup() != TickGroup::GAME) return;

	Ref<TransformComponent> targetComponent = root;
	Transform TargetTransform = targetComponent->GetWorldTransform();
	vec3d MovementVec = {0,0,0};
	//input
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_W))
	{
		MovementVec.z += MovementSpeed * tick.DeltaTime;
	}
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_S))
	{
		MovementVec.z -= MovementSpeed * tick.DeltaTime;
	}
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_A))
	{
		MovementVec.x -= MovementSpeed * tick.DeltaTime;
	}
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_D))
	{
		MovementVec.x += MovementSpeed * tick.DeltaTime;
	}

	if(!World::IsNearlyZero(MovementVec))
	{ 
		if(FlipForwardVector)
			MovementVec *= -1.f;

		//TargetRotation = World::VectorToRotation(MovementVec);
		TargetRotation = MovementVec;
	}
	glm::quat CurrentRot = World::RotationDegreesToQuat(mesh->GetWorldRotation());
	glm::quat TargetRot = World::VectorDirToQuat(TargetRotation);
	glm::quat lerpQuat = World::LerpQuat(CurrentRot, TargetRot, RoatationLerpSpeed * tick.DeltaTime);

	//vec3d lerpVec = World::InterpRot(mesh->GetWorldRotation(), TargetRotation, tick.DeltaTime, RoatationLerpSpeed);
	vec3d lerpVec = World::QuatToRotationDegrees(lerpQuat);
	mesh->SetWorldRotation(lerpVec);

	if (!World::IsNearlyZero(MovementVec))
	{
		root->SetWorldPosition(root->GetWorldPostition() + World::GetForwardVector(mesh->GetWorldRotation()) * MovementSpeed * tick.DeltaTime * (FlipForwardVector ? -1.f : 1.f));
	}
}

void PlayerController::OnBeginPlay()
{
	Super::OnBeginPlay();

	mouseEvent.Assign([&](MouseEvent* event)
	{
		if(event->GetEventType() == MouseEventType::BUTTON && event->GetKeyCode() == GLFW_MOUSE_BUTTON_LEFT)
		{
			SceneUtils::SpawnPrefabInScene(BulletPrefab, GetScene(), mesh->GetWorldTransform(), ObjectInitializer::Module(this));
		}		
	});

	GetScene()->GetInputManager().BindOnMouseDown(mouseEvent);
}
