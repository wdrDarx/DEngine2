#pragma once
#include "CustomClasses.h"
#include "DEngine.h"

class CUSTOMCLASSES CameraComponent : public TransformComponent
{
public:
	OBJECT_CLASS_DEF(CameraComponent, TransformComponent)
	OBJECT_PROPS_BEGIN()
		PROPS_CATEGORY(Camera)
		Projection.Value() = ProjectionType::Enum::PERSPECTIVE;
		PROPDEF(FOV, EditAnywhere);
		PROPDEF(Projection, EditAnywhere);
	OBJECT_PROPS_END()

public:
	void OnUpdate(const Tick& tick) override;
	void OnBeginPlay() override;


	ProjectionType Projection;

	ProjectionType::Enum m_LastProjectionType;
	Ref<Camera> m_Camera;
	float FOV = 90.f;
};

