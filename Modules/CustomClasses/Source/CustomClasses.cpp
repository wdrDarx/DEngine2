#include "CustomClasses.h"

#include "Classes/CustomAppObject.h"
#include "Classes/CustomSceneObject.h"
#include "Classes/ChildSceneObject.h"

#include "Classes/QuadRenderer.h"

void CustomClasses::OnLoad()
{
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);

	//custom renderer
	REGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadRenderer, CustomClasses);
	m_SceneEvent.Assign([&](SceneEvent* event)
	{
		if (event->GetEventType() == SceneEventType::SCENE_ONCONSTRUCT)
		{
			if (!gladLoadGL())
			{
				std::cout << "Failed to initialize OpenGL context" << std::endl;
			}

			event->GetScene()->CreateRenderer<QuadRenderer>(ObjectInitializer::Module("CustomClasses"));
		}
	});
	GetApplication()->GetEventDispatcher().Bind(m_SceneEvent);

	REGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct);
	REGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct2);
}

void CustomClasses::OnUnload()
{
	m_SceneEvent.Destroy();
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomAppObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), CustomSceneObject, CustomClasses);
	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), ChildSceneObject, CustomClasses);

	UNREGISTER_OBJECT(GetApplication()->GetObjectRegistry(), QuadRenderer, CustomClasses);

	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct);
	UNREGISTER_STRUCT(GetApplication()->GetStructRegistry(), TestStruct2);
}
