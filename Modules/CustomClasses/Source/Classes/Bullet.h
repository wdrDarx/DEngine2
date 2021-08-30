#pragma once
#include "CustomClasses.h"
#include "StaticMeshComponent.h"
#include "DEngine.h"

class CUSTOMCLASSES Bullet : public SceneObject
{
	public:
		OBJECT_CLASS_DEF(Bullet, SceneObject)
		OBJECT_PROPS_BEGIN()
			PROPS_CATEGORY(Bullet);
			PROPDEF(Speed, EditAnywhere);
		OBJECT_PROPS_END()

		void OnConstruct() override;
		void OnPostConstruct() override;
		void OnUpdate(const Tick& tick) override;

	public:
		Ref<TransformComponent> Root;
		Ref<StaticMeshComponent> mesh;
		float Speed = 100.f;
};

