#include "ObjectAssetEditor.h"

ObjectAssetEditor::ObjectAssetEditor() : AssetEditor("ObjectAsset")
{

}

void ObjectAssetEditor::Render()
{	
	title = m_TargetAsset->GetAssetName() + " Object Editor";
	bool isOpen = true;
	ImGui::Begin(title.c_str(), &isOpen, ImGuiWindowFlags_NoDocking);

	if (ImGui::Button("Save"))
	{
		m_ObjectAsset->SaveObjectProps(m_Object);
		m_App->GetAssetManager().SaveAsset(m_ObjectAsset, m_TargetAsset);
	}
	ImGui::SameLine();
	if (ImGui::Button("Set Base Class"))
	{
		ImGui::OpenPopup("Select Class");
	}

	if (ImGui::BeginPopup("Select Class"))
	{
		for (auto& key : m_App->GetObjectRegistry().GetRegisteredKeys())
		{
			if (ImGui::MenuItem(key.name.c_str()))
			{
				m_ObjectAsset->SetObjectClass(key.name);
				UpdateObjectClass();
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}
	ImGui::SameLine();
	std::string TextString = "Base Class : " + m_ObjectAsset->GetObjectClassName();
	ImGui::Text(TextString.c_str());

	if(m_Object)
	{ 
		bool expanded = ImGui::TreeNodeEx((void*)(m_Object.get()), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen, "Properties");
		if (expanded)
		{
			m_PropertyWindow.ListProperties(m_Object->GetPropertiesMutable(), m_Object);
			ImGui::TreePop();
		}
	}

	ImGui::End();
	if (!isOpen)
	{
		m_App->RemoveAssetEditor(this);
	}
}

void ObjectAssetEditor::Init()
{
	m_ObjectAsset = m_App->GetAssetManager().LoadAsset<ObjectAsset>(m_TargetAsset);
	m_PropertyWindow.m_App = m_App;

	if (!m_ObjectAsset) return;

	UpdateObjectClass();
	if (m_Object)
	{
		m_ObjectAsset->LoadObjectProps(m_Object);
	}
}

void ObjectAssetEditor::UpdateObjectClass()
{
	m_Object = ToRef<ObjectBase>(m_App->GetObjectRegistry().MakeObjectFromClassName(m_ObjectAsset->GetObjectClassName()));
	if(!m_Object) return; //not registered or no class name available
	m_Object->DefineProperties();
	m_App->GetAssetManager().SaveAsset(m_ObjectAsset, m_TargetAsset);
}
