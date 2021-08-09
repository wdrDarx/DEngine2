#pragma once
#include "DEngine.h"

class EditorApp;
class SceneObjectPannel
{
public:
	void Render(Ref<Scene> scene, bool DrawimGui = true);

	void DrawSceneObjectNode(Ref<SceneObject> object);
	void DrawComponentNode(Ref<ObjectComponent> comp);

	Ref<ObjectComponent> m_SelectedComponent;
	Ref<ObjectComponent> m_ComponentToDelete;
	Ref<SceneObject> m_ObjectToDelete;
	Ref<SceneObject> m_SelectedObject;
	Ref<Scene> m_Scene;
};