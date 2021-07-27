#include "SceneObjectPannel.h"
#include "ImGui/ImGuiLayer.h"

void SceneObjectPannel::Render(Ref<Scene> scene)
{
	m_Scene = scene;

	ImGui::Begin("Scene Objects");

	for (auto& obj : scene->GetSceneObjects())
	{
		DrawSceneObjectNode(obj);
	}


	//delete stuff
	if(m_ObjectToDelete)
	{ 
		m_Scene->DestroySceneObject(m_ObjectToDelete);
		m_ObjectToDelete = nullptr;
	}

	if (m_ComponentToDelete)
	{
		m_ComponentToDelete->GetOwner()->DestroyComponent(m_ComponentToDelete);
		m_ComponentToDelete = nullptr;
	}

	ImGui::End();
}

void SceneObjectPannel::DrawSceneObjectNode(Ref<SceneObject> object)
{
	ImGuiTreeNodeFlags flags = (m_SelectedObject == object ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	std::string name = object->GetName();
// 	if (m_Debug)
// 		name += "_" + std::to_string(obj->GetID());

	bool expanded = ImGui::TreeNodeEx((void*)object->GetID().ID, flags, name.c_str());
	if (ImGui::IsItemClicked())
	{
		m_SelectedObject = object;
		m_SelectedComponent = nullptr;
	}

	bool ObjectDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
// 		if (ImGui::MenuItem("Save Object"))
// 		{
// 			SceneUtils::SaveSceneObjectToFile(obj, FileDialog::SaveFile("SceneObject (*.sceneobject)\0*.sceneobject\0"));
// 		}

		if (ImGui::MenuItem("Destroy Object"))
			ObjectDeleted = true;

		ImGui::EndPopup();
	}

	if (expanded)
	{
		for (auto& comp : object->GetComponents())
		{
			DrawComponentNode(comp);
		}
		ImGui::TreePop();
	}

	if (ObjectDeleted)
	{
		if (object == m_SelectedObject)
		{
			m_SelectedObject = nullptr;
			m_SelectedComponent = nullptr;
		}
		m_ObjectToDelete = object;

	}
}

void SceneObjectPannel::DrawComponentNode(Ref<ObjectComponent> comp)
{
	ImGuiTreeNodeFlags flags = (m_SelectedComponent == comp ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	std::string name = comp->GetName();
// 	if (m_Debug)
// 		name += "_" + std::to_string(comp->GetID().ID);

	bool expanded = ImGui::TreeNodeEx((void*)comp->GetID().ID, flags, name.c_str());
	//TransformComponent* tcomp = dynamic_cast<TransformComponent*>(comp);

	if (ImGui::IsItemClicked())
	{
// 		if (m_WantsToAttach && tcomp)
// 		{
// 			m_WantsToAttach->AttachTo(tcomp);
// 			m_WantsToAttach = nullptr;
// 		}
// 		if (m_ComponentReferenceContext)
// 		{
// 			m_ComponentReferenceContext->MakeReference(comp);
// 			m_ComponentReferenceContext = nullptr;
// 		}
// 		else
			m_SelectedComponent = comp;
			m_SelectedObject = nullptr;
	}

	bool CompDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
// 
// 		if (tcomp && !tcomp->IsRootComponent())
// 		{
// 			if (tcomp->GetParentComponent())
// 			{
// 				if (ImGui::MenuItem("Detach"))
// 				{
// 					tcomp->Detach();
// 				}
// 			}
// 			else
// 			{
// 				if (ImGui::MenuItem("Attach To"))
// 				{
// 					m_WantsToAttach = tcomp;
// 				}
// 				if (ImGui::MenuItem("Make Root"))
// 				{
// 					tcomp->GetOwner()->SetRootComponent(tcomp);
// 				}
// 			}
// 		}
		if (ImGui::MenuItem("Destroy Component"))
			CompDeleted = true;

		ImGui::EndPopup();
	}

	if (expanded)
	{
// 		if (tcomp)
// 		{
// 			for (auto& attached : tcomp->GetChildren())
// 			{
// 				DrawObjectComponentNode(attached);
// 			}
// 		}
		ImGui::TreePop();
	}

	if (CompDeleted)
	{
		if (comp == m_SelectedComponent)
		{
			m_SelectedObject = nullptr;
			m_SelectedComponent = nullptr;
		}
		m_ComponentToDelete = comp;
	}
}