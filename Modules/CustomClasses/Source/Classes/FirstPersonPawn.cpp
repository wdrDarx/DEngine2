#include "FirstPersonPawn.h"
#include "Bullet.h"

void FirstPersonPawn::OnConstruct()
{
	Super::OnConstruct();

	collider = CreateComponent<BoxColliderComponent>(ObjectInitializer::Module(this), "Collider");
	mesh = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Mesh");
	camera = CreateComponent<CameraComponent>(ObjectInitializer::Module(this), "Camera");
	BulletSpawn = CreateComponent<TransformComponent>(ObjectInitializer::Module(this), "BulletSpawn");
	gun = CreateComponent<StaticMeshComponent>(ObjectInitializer::Module(this), "Gun");

	SetRootComponent(collider);
	mesh->AttachTo(collider);
	BulletSpawn->AttachTo(mesh);
	camera->AttachTo(mesh);
	gun->AttachTo(camera);

	collider->Kinamtic = false;
	collider->Dynamic = true;
	collider->CCD = false;
	collider->Lerp = true;
	collider->LockRotX = true;
	collider->LockRotY = true;
	collider->LockRotZ = true;
}

void FirstPersonPawn::OnUpdate(const Tick& tick)
{
	Super::OnUpdate(tick);

	if (tick.GetTickGroup() != TickGroup::GAME) return;

	vec3d RotationDelta = vec3d{ m_MouseMoveVector.y, -m_MouseMoveVector.x, 0 } * RotationSpeed * tick.DeltaTime;
	m_MouseMoveVector = { 0,0 };

	if(!World::IsNearlyZero(RotationDelta))
	{ 
		m_TargetRotation += RotationDelta;
		m_TargetRotation.x = glm::clamp(m_TargetRotation.x, -90.f, 90.f);
		m_TargetRotation = World::NormalizeRot(m_TargetRotation);
	}

	int RR = 30;
	m_ControlRotation = World::LerpRot(m_ControlRotation, m_TargetRotation, tick.DeltaTime * RR);
	camera->SetWorldRotation(m_ControlRotation);

	vec3d MeshRot = mesh->GetLocalTransform().rot;
	MeshRot.y = -m_ControlRotation.y;
	mesh->SetLocalRotation(MeshRot);

	float ForwardMovement = 0.0f;
	float RightMovement = 0.0f;
	//input
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_W))
	{
		ForwardMovement += MovementSpeed;
	}
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_S))
	{
		ForwardMovement -= MovementSpeed;
	}
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_A))
	{
		RightMovement -= MovementSpeed;
	}
	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_D))
	{
		RightMovement += MovementSpeed;
	}


	if (GetScene()->GetInputManager().IsKeyDown(GLFW_KEY_E))
	{
		collider->GetPhysicsActor()->AddForce(World::GetForwardVector(mesh->GetWorldRotation()) * BlinkSpeed * tick.DeltaTime, ForceMode::VelocityChange);
	}

	if (!World::IsNearlyZero(ForwardMovement) || !World::IsNearlyZero(RightMovement))
	{
		vec3d Forward = World::GetForwardVector(m_ControlRotation * vec3d{ 0,1,0 }) * ForwardMovement * tick.DeltaTime;
		vec3d Right = World::GetRightVector(m_ControlRotation * vec3d{ 0,1,0 }) * RightMovement * tick.DeltaTime;
		Forward.y = 0;
		Right.y = 0;

		collider->GetPhysicsActor()->AddForce((Right + Forward), ForceMode::VelocityChange);
	}
	else
	{
		//cancel out velocity if not moving
		vec3d velocity = collider->GetPhysicsActor()->GetLinearVelocity();
		velocity.y = 0.f;
		collider->GetPhysicsActor()->AddForce(-velocity * tick.DeltaTime, ForceMode::VelocityChange);
	}

	vec3d velocity = collider->GetPhysicsActor()->GetLinearVelocity();
	velocity.x = glm::clamp(velocity.x, -(MovementSpeed), (MovementSpeed));
	velocity.z = glm::clamp(velocity.z, -(MovementSpeed), (MovementSpeed));
	collider->GetPhysicsActor()->SetLinearVelocity(velocity);
}

void FirstPersonPawn::OnBeginPlay()
{
	Super::OnBeginPlay();

	m_PhysicsCallback.Assign([&](PhysicsActorEvent* event)	
	{
		if (event->GetEventType() == PhysicsActorEventType::ONHIT)
		{
			if( glm::dot(event->Hit.HitNormal, {0,1,0}) > 0.9f);
				m_Grounded = true;
		}
	});

	collider->GetPhysicsActor()->BindOnHit(m_PhysicsCallback);

	MouseMoveCallback.Assign([&](MouseEvent* event)
	{
		if (event->GetEventType() == MouseEventType::MOVED)
		{
			//m_MouseMoveVector = GetSmoothCameraRot(GetScene()->GetLastTick().DeltaTime, 50, event->GetMoveVector());			
			m_MouseMoveVector = event->GetMoveVector();
		}
	});


	MouseDownCallback.Assign([&](MouseEvent* event)
	{
		if (event->GetEventType() == MouseEventType::BUTTON)
		{
			if (event->GetKeyCode() == GLFW_MOUSE_BUTTON_LEFT && BulletPrefab.IsValid())
			{
				Transform spawn = BulletSpawn->GetWorldTransform();
				spawn.rot = camera->GetWorldRotation();
				if (auto bullet = Cast<Bullet>(SceneUtils::SpawnPrefabInScene(BulletPrefab, GetScene(), spawn, ObjectInitializer::Module(this), true)))
				{
					//bullet->Root->GetPhysicsActor()->AddForce(World::GetForwardVector(m_ControlRotation) * BulletForce * GetScene()->GetLastTick().DeltaTime, ForceMode::VelocityChange);
				}
			}
		}
	});

	keyEvent.Assign([&](KeyEvent* event)
	{
		if (event->GetKeyCode() == GLFW_KEY_SPACE && m_Grounded)
		{
			collider->GetPhysicsActor()->AddForce({0,JumpHeight,0}, ForceMode::VelocityChange);
			m_Grounded = false;
		}

	});

	GetScene()->GetInputManager().BindOnMouseMove(MouseMoveCallback);
	GetScene()->GetInputManager().BindOnMouseDown(MouseDownCallback);
	GetScene()->GetInputManager().BindOnKeyDown(keyEvent);

}

vec2d FirstPersonPawn::GetSmoothCameraRot(float deltaTime, float Springiness, const vec2d& MouseDelta)
{
	vec2d out = MouseDelta;
	double d = 1 - exp(log(0.5) * Springiness * deltaTime);

	out.x = (MouseDelta.x) * d;
	out.y = (MouseDelta.y) * d;
	return out;
}
