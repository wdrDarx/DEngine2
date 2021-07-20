#pragma once
#include "DEngine.h"
#include "ImGui/ImGuiLayer.h"


class PropertyWindow
{
	public:
		void Render()
		{
			ImGui::Begin("Properties");

			if (m_SelectedSceneObject)
			{
				DrawSceneObject(m_SelectedSceneObject);
			}

			ImGui::End();
		}

		void DrawSceneObject(Ref<SceneObject> sceneObject)
		{			
			ListProperties(sceneObject->GetProperties());
		}

		void ListProperties(const std::vector<Property>& props)
		{
			for (const auto& prop : props)
			{
				if(!(prop.m_Flags & PropFlags::EditAnywhere)) continue;

				//ImGui::Text(prop.m_name.c_str());
				//ImGui::SameLine();

				switch (prop.m_Type)
				{
					case PropType::BOOL :
					{
						ImGui::Checkbox(prop.m_name.c_str(), (bool*)prop.m_Value);
						break;
					}

					case PropType::INT :
					{
						ImGui::DragInt(prop.m_name.c_str(), (int*)prop.m_Value);
						break;
					}

					case PropType::FLOAT:
					{
						ImGui::DragFloat(prop.m_name.c_str(), (float*)prop.m_Value);
						break;
					}

					case PropType::STRING:
					{
						ImGui::InputText(prop.m_name.c_str(), (std::string*)prop.m_Value);
						break;
					}

					case PropType::VEC2D:
					{
						float drag2[2];
						drag2[0] = ((vec2d*)prop.m_Value)->x; 
						drag2[1] = ((vec2d*)prop.m_Value)->y;
						ImGui::DragFloat2(prop.m_name.c_str(), drag2);

						((vec2d*)prop.m_Value)->x = drag2[0];
						((vec2d*)prop.m_Value)->y = drag2[1];
						break;
					}

					case PropType::VEC3D:
					{
						float drag3[3];
						drag3[0] = ((vec3d*)prop.m_Value)->x;
						drag3[1] = ((vec3d*)prop.m_Value)->y;
						drag3[2] = ((vec3d*)prop.m_Value)->z;				
						ImGui::DragFloat3(prop.m_name.c_str(), drag3);

						((vec3d*)prop.m_Value)->x = drag3[0];
						((vec3d*)prop.m_Value)->y = drag3[1];
						((vec3d*)prop.m_Value)->z = drag3[2];
						break;
					}
				}
			}
		}
public:
	Ref<SceneObject> m_SelectedSceneObject;
};