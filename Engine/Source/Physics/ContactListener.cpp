#include "DEngine.h"
#include "ContactListener.h"
#include "PhysicsActor.h"


void ContactListener::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
	PX_UNUSED(constraints);
	PX_UNUSED(count);
}

void ContactListener::onWake(physx::PxActor** actors, physx::PxU32 count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		physx::PxActor& physxActor = *actors[i];
		PhysicsActor* actor = (PhysicsActor*)physxActor.userData;
	}
}

void ContactListener::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
	for (uint32_t i = 0; i < count; i++)
	{
		physx::PxActor& physxActor = *actors[i];
		PhysicsActor* actor = (PhysicsActor*)physxActor.userData;
	}
}

void ContactListener::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	PhysicsActor* actorA = (PhysicsActor*)(pairHeader.actors[0]->userData);
	PhysicsActor* actorB = (PhysicsActor*)(pairHeader.actors[1]->userData);

	if (pairs->flags == physx::PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH)
	{
// 		if (ScriptEngine::IsEntityModuleValid(actorA->GetEntity())) ScriptEngine::OnCollisionBegin(actorA->GetEntity());
// 		if (ScriptEngine::IsEntityModuleValid(actorB->GetEntity())) ScriptEngine::OnCollisionBegin(actorB->GetEntity());
	}
	else if (pairs->flags == physx::PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH)
	{
// 		if (ScriptEngine::IsEntityModuleValid(actorA->GetEntity())) ScriptEngine::OnCollisionEnd(actorA->GetEntity());
// 		if (ScriptEngine::IsEntityModuleValid(actorB->GetEntity())) ScriptEngine::OnCollisionEnd(actorB->GetEntity());
	}
}

void ContactListener::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	PhysicsActor* actorA = (PhysicsActor*)pairs->triggerActor->userData;
	PhysicsActor* actorB = (PhysicsActor*)pairs->otherActor->userData;

	if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
	{
// 		if (ScriptEngine::IsEntityModuleValid(actorA->GetEntity())) ScriptEngine::OnTriggerBegin(actorA->GetEntity());
// 		if (ScriptEngine::IsEntityModuleValid(actorB->GetEntity())) ScriptEngine::OnTriggerBegin(actorB->GetEntity());
	}
	else if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
	{
// 		if (ScriptEngine::IsEntityModuleValid(actorA->GetEntity())) ScriptEngine::OnTriggerEnd(actorA->GetEntity());
// 		if (ScriptEngine::IsEntityModuleValid(actorB->GetEntity())) ScriptEngine::OnTriggerEnd(actorB->GetEntity());
	}
}

void ContactListener::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
	PX_UNUSED(bodyBuffer);
	PX_UNUSED(poseBuffer);
	PX_UNUSED(count);
}