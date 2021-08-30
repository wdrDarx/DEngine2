#pragma once
#include "DEngine.h"

class EditorApp;
class SceneObjectPannel
{
public:
	void Render(Scene* scene, bool DrawimGui = true);

	void DrawSceneObjectNode(SceneObject* object);
	void DrawComponentNode(ObjectComponent* comp);

	ObjectComponent* m_SelectedComponent = nullptr;
	ObjectComponent* m_ComponentToDelete = nullptr;
	SceneObject* m_ObjectToDelete = nullptr;
	SceneObject* m_SelectedObject = nullptr;
	Scene* m_Scene;
};