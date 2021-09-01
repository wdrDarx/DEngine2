#pragma once
#include "ColliderComponent.h"

class DENGINE_API BoxColliderComponent : public ColliderComponent
{
public:	
	OBJECT_CLASS_DEF(BoxColliderComponent, ColliderComponent);
	OBJECT_PROPS_BEGIN()
		PROPS_CATEGORY(BoxCollider)
			PROPDEF(BoxSize, EditAnywhere);
	OBJECT_PROPS_END()

	void OnBeginPlay() override;
	void OnEndPlay() override;
	void OnUpdate(const Tick& tick) override;

	Ref<ColliderShape> CreateColliderShape() override;
	vec3d BoxSize = {100,100,100};
};

