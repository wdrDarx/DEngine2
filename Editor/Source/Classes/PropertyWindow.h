#pragma once
#include "DEngine.h"
#include "ImGui/ImGuiLayer.h"


struct CategoryPair
{
	std::string Category;
	Property* Prop;
};

struct MyStruct {
	int key;
	std::string data;
	MyStruct(int key, std::string data) {
		this->key = key;
		this->data = data;
	}
};

class PropertyWindow
{
	public:
		void Init(Ref<Application> app)
		{
			m_App = app;
		}

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
			ListProperties(sceneObject->GetPropertiesMutable(), sceneObject);
		}

		void BeginCategory(const std::string& CategoryName)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 2,2 });
			bool expanded = ImGui::TreeNodeEx((void*)(CategoryName.c_str()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed, CategoryName.c_str());
			if (expanded)
			{
				
				ImGui::TreePop();
			}
			ImGui::PopStyleVar();
		}

		void ListProperties(std::vector<Property>& props, Ref<ObjectBase> owner = nullptr, bool ForceNewLine = false, bool DrawCategories = true)
		{
			if(DrawCategories)
			{ 
				if(props.size() < 1) return;

				std::vector<CategoryPair> t_pairs;
				for (auto& prop : props)
				{
					if(!(prop.m_Flags & PropFlags::EditAnywhere)) continue;

					std::string key = prop.m_category.empty() ? "Default" : prop.m_category;
					t_pairs.push_back({key, &prop});
				}

				//sort by category
				cluster(t_pairs.begin(), t_pairs.end(), [](const CategoryPair& first, const CategoryPair& second)
				{
					return first.Category == second.Category;
				});

				std::string CurrectCategory;
				bool LastCategoryExpanded = false;
				uint CategoryIDindex = 21;
				for (uint i = 0; i < t_pairs.size(); i++)
				{
					auto& pair = t_pairs[i];

					if (pair.Category != CurrectCategory)
					{
						if(LastCategoryExpanded)
							ImGui::TreePop();

						CurrectCategory = pair.Category;
						
						//Draw category thing here
						LastCategoryExpanded = ImGui::TreeNodeEx((void*)(CategoryIDindex), ImGuiTreeNodeFlags_Framed, CurrectCategory.c_str());
						CategoryIDindex++;
					}
					if (LastCategoryExpanded)
					{
						Property& prop = *pair.Prop;
						DrawProperty(prop, owner);
					
						//we at at the end - pop the tree
						if(i == t_pairs.size() - 1)
							ImGui::TreePop();
					}
				}
			}
			else
			{
				for (auto& prop : props)
				{
					DrawProperty(prop, owner);
				}
			}
		}

		void DrawProperty(Property& prop, Ref<ObjectBase> owner = nullptr, bool ForceNewLine = false)
		{
			std::string DisplayName = prop.m_name;
			ImGui::PushID(prop.m_Value);

			//exists cuz im dumb and idk how to make structs look good
			if (ForceNewLine)
				ImGui::NewLine();

			if (prop.m_Type == PropType::DSTRUCT)
			{
				ImGui::NewLine();
				DStruct* structToDraw = (DStruct*)prop.m_Value;
				ImGui::Text(prop.m_name.c_str());
				ImGui::SameLine();

				ImGui::PushItemWidth(ImGui::CalcItemWidth());
				bool expanded = ImGui::TreeNodeEx((void*)(prop.m_Value), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap, structToDraw->GetClassType().Name.c_str());
				ImGui::PopItemWidth();

				ImGui::SameLine();
				float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight, lineHeight };

				//Reset button TODO add reset button to struct properties too
				if (owner && ImGui::Button("<", buttonSize))
				{
					ObjectUtils::ResetObjectProp(owner, prop.m_name, m_App->GetRegistry());
				}

				if (expanded)
				{
					ListProperties(structToDraw->GetPropertiesMutable(), nullptr, true, false);
					ImGui::TreePop();
				}
				
			}
			else
			{
				ImGui::Columns(2);
				ImGui::Text(DisplayName.c_str());
				ImGui::SetColumnOffset(1, 100);

				ImGui::NextColumn();

				ImGui::PushItemWidth(ImGui::CalcItemWidth());

				DisplayName = "";
				DStruct* structToDraw = nullptr;
				switch (prop.m_Type)
				{
				case PropType::BOOL:
				{
					ImGui::Checkbox(DisplayName.c_str(), (bool*)prop.m_Value);
					break;
				}

				case PropType::INT:
				{
					ImGui::DragInt(DisplayName.c_str(), (int*)prop.m_Value);
					break;
				}

				case PropType::FLOAT:
				{
					ImGui::DragFloat(DisplayName.c_str(), (float*)prop.m_Value);
					break;
				}

				case PropType::STRING:
				{
					ImGui::InputText(DisplayName.c_str(), (std::string*)prop.m_Value);
					break;
				}

				case PropType::VEC2D:
				{
					float drag2[2];
					drag2[0] = ((vec2d*)prop.m_Value)->x;
					drag2[1] = ((vec2d*)prop.m_Value)->y;
					ImGui::DragFloat2(DisplayName.c_str(), drag2);

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
					DrawVec3Control(prop.m_Value, drag3);
					((vec3d*)prop.m_Value)->x = drag3[0];
					((vec3d*)prop.m_Value)->y = drag3[1];
					((vec3d*)prop.m_Value)->z = drag3[2];
					break;
				}
				}
				ImGui::PopItemWidth();
				ImGui::EndColumns();

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::CalcItemWidth());
				float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight, lineHeight };

				//Reset button TODO add reset button to struct properties too
				if (owner && prop.m_Type != PropType::DSTRUCT && ImGui::Button("<", buttonSize))
				{
					ObjectUtils::ResetObjectProp(owner, prop.m_name, m_App->GetRegistry());
				}

				ImGui::PopItemWidth();

				
			}

			ImGui::PopID();
		}


		static void DrawVec3Control(void* ID, float* value, float ResetValue = 0.f, const float& Speed = 1.0f, float columnWidth = 100.f)
		{
			auto BoldFont = ImGui::GetIO().Fonts->Fonts[0];

			ImGui::PushID(ID);

			ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

			float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f,1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f,1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f,1.0f });
			ImGui::PushFont(BoldFont);
			if (ImGui::Button("X", buttonSize))
				value[0] = ResetValue;
			ImGui::PopFont();

			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##X", &value[0], Speed);
			ImGui::PopItemWidth();
			ImGui::SameLine();


			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f,1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
			ImGui::PushFont(BoldFont);
			if (ImGui::Button("Y", buttonSize))
				value[1] = ResetValue;
			ImGui::PopFont();

			ImGui::SameLine();
			ImGui::DragFloat("##Y", &value[1], Speed);
			ImGui::PopItemWidth();
			ImGui::SameLine();

			ImGui::PopStyleColor(3);

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f,1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f,1.0f });
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f,1.0f });
			ImGui::PushFont(BoldFont);
			if (ImGui::Button("Z", buttonSize))
				value[2] = ResetValue;
			ImGui::PopFont();

			ImGui::PopStyleColor(3);

			ImGui::SameLine();
			ImGui::DragFloat("##Z", &value[2], Speed);
			ImGui::PopItemWidth();

			ImGui::PopStyleVar();

			ImGui::PopID();
		}
public:
	Ref<SceneObject> m_SelectedSceneObject;
	Ref<Application> m_App;
	float m_ColumnWidth = 100.f;
};