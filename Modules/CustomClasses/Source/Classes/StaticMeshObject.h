#pragma once
#include "CustomClasses.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "DEngine.h"

class StaticMeshObject : public SceneObject
{
public:
	OBJECT_CLASS_DEF(StaticMeshObject, SceneObject)
	OBJECT_PROPS_BEGIN()
	OBJECT_PROPS_END()

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

public:
	Ref<StaticMeshComponent> m_Mesh;
};
