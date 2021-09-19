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
	PX_UNUSED(actors);
	PX_UNUSED(count);
}

void ContactListener::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
	PX_UNUSED(actors);
	PX_UNUSED(count);
}

void ContactListener::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	PhysicsActor* actorA = (PhysicsActor*)(pairHeader.actors[0]->userData);
	PhysicsActor* actorB = (PhysicsActor*)(pairHeader.actors[1]->userData);

	if (!actorA->ShouldReceiveCollisionCallbacks() && !actorB->ShouldReceiveCollisionCallbacks())
	{
		PX_UNUSED(pairs);
		PX_UNUSED(nbPairs);
		return;
	}

	uint contactCount = pairs[0].contactCount;
	physx::PxContactPairPoint* contacts = new physx::PxContactPairPoint[contactCount];
	pairs[0].extractContacts(contacts, contactCount);

	HitResult hitA;
	hitA.OtherColliderComponent = actorB->GetColliderComponent();
	hitA.OtherPhysicsActor = actorB;
	hitA.HitPosition = PhysicsUtils::FromPhysXVector(contacts[0].position);
	hitA.HitNormal = PhysicsUtils::FromPhysXVector(contacts[0].normal);
	hitA.HitImpulse = PhysicsUtils::FromPhysXVector(contacts[0].impulse);

	HitResult hitB;
	hitB.OtherColliderComponent = actorA->GetColliderComponent();
	hitB.OtherPhysicsActor = actorA;
	hitB.HitPosition = PhysicsUtils::FromPhysXVector(contacts[1].position);
	hitB.HitNormal = PhysicsUtils::FromPhysXVector(contacts[1].normal);
	hitB.HitImpulse = PhysicsUtils::FromPhysXVector(contacts[1].impulse);

	delete[] contacts;

	if (pairs->flags & physx::PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH)
	{	
		actorA->CallOnHit(hitA);
		actorB->CallOnHit(hitB);

		if (actorA->GetOverlappingLayers() & BIT(actorB->GetCollisionLayer() + 1))
		{
			actorA->CallOnBeginOverlap(hitA);
		}

		if (actorB->GetOverlappingLayers() & BIT(actorA->GetCollisionLayer() + 1))
		{
			actorB->CallOnBeginOverlap(hitB);
		}
	}
	else if (pairs->flags & physx::PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH)
	{
		if (actorA->GetOverlappingLayers() & BIT(actorB->GetCollisionLayer() + 1))
		{
			actorA->CallOnEndOverlap(hitA);
		}

		if (actorB->GetOverlappingLayers() & BIT(actorA->GetCollisionLayer() + 1))
		{
			actorB->CallOnEndOverlap(hitB);
		}
	}


}

void ContactListener::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	PhysicsActor* actorA = (PhysicsActor*)pairs->triggerActor->userData;
	PhysicsActor* actorB = (PhysicsActor*)pairs->otherActor->userData;

	if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
	{
		ASSERT(false);
	}
	else if (pairs->status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
	{
		//frfr
	}
}

void ContactListener::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
	PX_UNUSED(bodyBuffer);
	PX_UNUSED(poseBuffer);
	PX_UNUSED(count);
}