#include "PhysicsScene.h"
#include "PhysicsWorld.h"
#include "PhysXAPI.h"
#include "ContactListener.h"
#include "DEngine.h"

PhysicsScene::PhysicsScene(PhysicsWorld* world, const PhysicsSettings& settings) : m_SubStepSize(settings.FixedTimestep), m_PhysicsWorld(world)
{
	physx::PxSceneDesc sceneDesc(world->GetPhysicsAPI()->GetPhysXSDK()->getTolerancesScale());
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD | physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS | physx::PxSceneFlag::eADAPTIVE_FORCE | physx::PxSceneFlag::eENABLE_PCM;// TODO: | physx::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS
	sceneDesc.gravity = PhysicsUtils::ToPhysXVector(settings.Gravity);
	sceneDesc.broadPhaseType = PhysicsUtils::ToPhysXBroadphaseType(settings.BroadphaseAlgorithm);
	sceneDesc.cpuDispatcher = world->GetPhysicsAPI()->GetCPUDispatcher();
	sceneDesc.filterShader = (physx::PxSimulationFilterShader)PhysicsUtils::FilterShader;
	sceneDesc.simulationEventCallback = &m_ContactListener;
	sceneDesc.frictionType = PhysicsUtils::ToPhysXFrictionType(settings.FrictionModel);

	//gpu dispatcher
	if(world->GetPhysicsAPI()->GetCudaContextManager())
	{ 
		sceneDesc.cudaContextManager = world->GetPhysicsAPI()->GetCudaContextManager();
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
	}

	ASSERT(sceneDesc.isValid());

	m_PhysXScene = world->GetPhysicsAPI()->GetPhysXSDK()->createScene(sceneDesc);
	ASSERT(m_PhysXScene);

	//debug
	m_PhysXScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_PhysXScene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
	m_PhysXScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);

	physx::PxPvdSceneClient* pvdClient = m_PhysXScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
}

PhysicsScene::~PhysicsScene()
{
	ASSERT(m_PhysXScene);

	for (auto& actor : m_Actors)
		RemoveActor(actor);

	m_Actors.clear();
	m_PhysXScene->release();
	m_PhysXScene = nullptr;
}

void PhysicsScene::Simulate(float ts, bool callFixedUpdate)
{
	bool advanced = Advance(ts);

	if (advanced)
	{
		uint32_t nbActiveActors;
		physx::PxActor** activeActors = m_PhysXScene->getActiveActors(nbActiveActors);
		for (uint32_t i = 0; i < nbActiveActors; i++)
		{
			PhysicsActor* actor = (PhysicsActor*)(activeActors[i]->userData);
			actor->OnAdvance();
		}
	}
}

void PhysicsScene::AddActor(Ref<PhysicsActor> actor)
{
	ASSERT(actor);
	ASSERT(actor->GetPhysXActor()); //you forgor to call CreateRigidActor()

	m_Actors.push_back(actor);
	m_PhysXScene->addActor(*actor->m_RigidActor);
	actor->SetSimulationData(actor->GetCollisionLayer());
}

void PhysicsScene::RemoveActor(Ref<PhysicsActor> actor)
{
	if (!actor)
		return;

	auto remove = std::find(m_Actors.begin(), m_Actors.end(), actor);
	if(remove != m_Actors.end())
		m_Actors.erase(remove);
}

void PhysicsScene::RemoveActor(PhysicsActor* actor)
{
	if (!actor)
		return;

	ObjectUtils::RemovePointerFromRefVector(actor, m_Actors);
}

bool PhysicsScene::Raycast(const vec3d& Start, const vec3d& End, RaycastHit* outHit)
{
	physx::PxRaycastBuffer hitInfo;
	bool result = m_PhysXScene->raycast(PhysicsUtils::ToPhysXVector(Start), PhysicsUtils::ToPhysXVector(glm::normalize(End - Start)), glm::length(End - Start), hitInfo);

	if (result)
	{
// 		outHit->Position = PhysicsUtils::FromPhysXVector(hitInfo.block.position);
// 		outHit->Normal = PhysicsUtils::FromPhysXVector(hitInfo.block.normal);
	}

	return result;
}

bool PhysicsScene::OverlapBox(const vec3d& origin, const vec3d& halfSize, std::array<physx::PxOverlapHit, OVERLAP_MAX_COLLIDERS>& buffer, uint32_t& count)
{
	return OverlapGeometry(origin, physx::PxBoxGeometry(halfSize.x, halfSize.y, halfSize.z), buffer, count);
}

void PhysicsScene::CreateRegions()
{
	const PhysicsSettings& settings = GetPhysicsWorld()->GetPhysicsSettings();

	if (settings.BroadphaseAlgorithm != BroadphaseType::AutomaticBoxPrune)
	{
// 		physx::PxBounds3* regionBounds = new physx::PxBounds3[(size_t)settings.WorldBoundsSubdivisions * settings.WorldBoundsSubdivisions];
// 		physx::PxBounds3 globalBounds(PhysicsUtils::ToPhysXVector(settings.WorldBoundsMin), PhysicsUtils::ToPhysXVector(settings.WorldBoundsMax));
// 		uint32_t regionCount = physx::PxBroadPhaseExt::createRegionsFromWorldBounds(regionBounds, globalBounds, settings.WorldBoundsSubdivisions);
// 
// 		for (uint32_t i = 0; i < regionCount; i++)
// 		{
// 			physx::PxBroadPhaseRegion region;
// 			region.bounds = regionBounds[i];
// 			m_PhysXScene->addBroadPhaseRegion(region);
// 		}
	}
}

bool PhysicsScene::Advance(float dt)
{
	SubstepStrategy(dt, m_NumSubSteps, m_SubStepSize);

	if (m_NumSubSteps == 0)
		return false;

	for (uint i = 0; i < m_NumSubSteps; i++)
	{
		m_PhysXScene->simulate(m_SubStepSize);
		m_PhysXScene->fetchResults(true);
	}

	return true;
}

void PhysicsScene::SubstepStrategy(float ts, uint32_t& substepCount, float& substepSize)
{
	if (m_Accumulator > m_SubStepSize)
		m_Accumulator = 0.0f;

	m_Accumulator += ts;
	if (m_Accumulator < m_SubStepSize)
	{
		substepCount = 0;
		return;
	}

	substepSize = m_SubStepSize;
	substepCount = glm::min(static_cast<uint>(m_Accumulator / m_SubStepSize), c_MaxSubSteps);

	m_Accumulator -= (float)substepCount * substepSize;
}

void PhysicsScene::Destroy()
{
	ASSERT(m_PhysXScene);

	for (auto& actor : m_Actors)
		RemoveActor(actor);

	m_Actors.clear();
	m_PhysXScene->release();
	m_PhysXScene = nullptr;
}

bool PhysicsScene::OverlapGeometry(const vec3d& origin, const physx::PxGeometry& geometry, std::array<physx::PxOverlapHit, OVERLAP_MAX_COLLIDERS>& buffer, uint32_t& count)
{
	physx::PxOverlapBuffer buf(buffer.data(), OVERLAP_MAX_COLLIDERS);
	physx::PxTransform pose = PhysicsUtils::ToPhysXTransform(glm::translate(glm::mat4(1.0f), origin));

	bool result = m_PhysXScene->overlap(geometry, pose, buf);

	if (result)
	{
		memcpy(buffer.data(), buf.touches, buf.nbTouches * sizeof(physx::PxOverlapHit));
		count = buf.nbTouches;
	}

	return result;
}
