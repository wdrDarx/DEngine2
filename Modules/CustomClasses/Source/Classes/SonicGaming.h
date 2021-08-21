#pragma once
#include "CustomClasses.h"
#include "StaticMeshComponent.h"
#include "DEngine.h"

class SonicGaming : public SceneObject
{
public:
	OBJECT_CLASS_DEF(SonicGaming, SceneObject)
	OBJECT_PROPS_BEGIN()
		PROPDEF(Speed, EditAnywhere);
	OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

public:
	Ref<StaticMeshComponent> m_MeshComponent;
	float Speed = 100.f;
};
