#pragma once
#include "Framework/Classes/Components/StaticMeshComponent.h"

class StaticMeshObject : public SceneObject
{
public:
	OBJECT_CLASS_DEF(StaticMeshObject, SceneObject)

	void OnUpdate(const Tick& tick) override;
	void OnConstruct() override;

public:
	Ref<StaticMeshComponent> m_Mesh;
};