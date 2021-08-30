#include "SceneObjectPannel.h"
#include "ImGui/ImGuiLayer.h"

void SceneObjectPannel::Render(Scene* scene, bool DrawimGui)
{
	m_Scene = scene;

	if(DrawimGui)
		ImGui::Begin("Scene Objects");

	for (auto& obj : scene->GetSceneObjects())
	{
		DrawSceneObjectNode(obj.get());
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

	if (DrawimGui)
		ImGui::End();
}

void SceneObjectPannel::DrawSceneObjectNode(SceneObject* object)
{
	ImGuiTreeNodeFlags flags = (m_SelectedObject == object ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

	std::string name = object->GetName();

	//prefab color
	ImGui::PushStyleColor(ImGuiCol_Text, object->IsPrefab() ? ImVec4{0.6f, 0.7f, 1.0f, 1.0f} : ImVec4{ 1.f, 1.f, 1.0f, 1.0f });
	bool expanded = ImGui::TreeNodeEx((void*)object->GetID().ID, flags, name.c_str());
	ImGui::PopStyleColor();
	if (ImGui::IsItemClicked())
	{
		m_SelectedObject = object;
		m_SelectedComponent = nullptr;
	}

	bool ObjectDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Destroy Object"))
			ObjectDeleted = true;

		if (object->IsPrefab())
		{
			if (ImGui::MenuItem("Reload Prefab"))
			{
				auto prefabAsset = m_Scene->GetApplication()->GetAssetManager().LoadAsset(object->GetPrefabAssetRefMutable());
				if(prefabAsset)
					prefabAsset->LoadPrefab(object, false, true);
			}
		}

		ImGui::EndPopup();
	}

	if (expanded)
	{
		if(auto& root = object->GetRootComponent())
			DrawComponentNode(root.get());

		for (auto& comp : object->GetComponents())
		{
			//draw all unattached transform components or just normal components
			if (!Cast<TransformComponent>(comp))
			{
				DrawComponentNode(comp.get());
			}
			else
				if(!Cast<TransformComponent>(comp)->GetParent() && !Cast<TransformComponent>(comp)->IsRootComponent())
					DrawComponentNode(comp.get());
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

void SceneObjectPannel::DrawComponentNode(ObjectComponent* comp)
{
	ImGuiTreeNodeFlags flags = (m_SelectedComponent == comp ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	std::string name = comp->GetName();

	bool expanded = ImGui::TreeNodeEx((void*)comp->GetID().ID, flags, name.c_str());

	if (ImGui::IsItemClicked())
	{
			m_SelectedComponent = comp;
			m_SelectedObject = nullptr;
	}

	bool CompDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Destroy Component"))
			CompDeleted = true;

		ImGui::EndPopup();
	}

	if (expanded)
	{
		TransformComponent* tcomp = Cast<TransformComponent>(comp);
		if (tcomp)
		{
			for (auto& attached : tcomp->GetChildren())
			{
				DrawComponentNode(attached.get());
			}
		}
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
