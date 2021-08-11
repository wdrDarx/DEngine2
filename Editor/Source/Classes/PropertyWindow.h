#pragma once
#include "DEngine.h"
#include "ImGui/ImGuiLayer.h"


struct CategoryPair
{
	std::string Category;
	Property* Prop;
};


class PropertyWindow
{
	public:
		void Init(Application* app)
		{
			m_App = app;
		}

		void Render(bool DrawimGui = true)
		{
			if(DrawimGui)
				ImGui::Begin("Properties");

			if (m_SelectedComponent)
			{
				DrawComponentProps(m_SelectedComponent);
			}
			else
			if (m_SelectedSceneObject)
			{
				DrawSceneObjectProps(m_SelectedSceneObject);
			}

			if (DrawimGui)
				ImGui::End();
		}

		void DrawSceneObjectProps(Ref<SceneObject> sceneObject)
		{		
			ImGui::Text("Name");
			ImGui::SameLine();
			ImGui::InputText("##Name", &sceneObject->GetNameMutable());

			if (ImGui::Button("Serialize/Reload"))
			{
				Buffer temp;
				sceneObject->Serialize(temp);
				sceneObject->Deserialize(temp);
			}
			ListProperties(sceneObject->GetPropertiesMutable(), sceneObject);
		}

		void DrawComponentProps(Ref<ObjectComponent> Comp)
		{
			if (ImGui::Button("Serialize/Reload"))
			{
				Buffer temp;
				Comp->Serialize(temp);
				Comp->Deserialize(temp);
			}
			ListProperties(Comp->GetPropertiesMutable(), Comp);
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

		void ResetProperty(ObjectBase* object, const std::string& propName, ObjectRegistry& objRegistry)
		{
			SceneObject* sceneObj = Cast<SceneObject>(object);
			if (sceneObj && sceneObj->IsPrefab())
			{
				//reset prop from prefab asset defaults
				ObjectUtils::ResetObjectProp(sceneObj, propName, sceneObj->GetPrefabAssetRef());
			}
			else
			{
				//reset from c++ class defaults
				ObjectUtils::ResetObjectProp(object, propName, objRegistry);
			}
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
				uint CategoryIDindex = 0;
				for (uint i = 0; i < t_pairs.size(); i++)
				{
					auto& pair = t_pairs[i];

					if (pair.Category != CurrectCategory)
					{
						if(LastCategoryExpanded)
							ImGui::TreePop();

						CurrectCategory = pair.Category;
						
						//Draw category thing here
						LastCategoryExpanded = ImGui::TreeNodeEx((void*)(CategoryIDindex), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen, CurrectCategory.c_str());
						CategoryIDindex++;
					}
					if (LastCategoryExpanded)
					{
						Property& prop = *pair.Prop;
						DrawProperty(prop, owner, ForceNewLine);
					
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
					DrawProperty(prop, owner, ForceNewLine);
				}
			}
		}

		void DrawAssetRef(AssetRef<Asset>* assetRef)
		{
			Ref<AssetHandle> handle = assetRef->GetAssetHandle();
			std::string buttonText = handle->IsValid() ? "(" + handle->GetAssetType() + ") " + handle->GetAssetName() : "None";

			if (ImGui::Button(buttonText.c_str()))
				ImGui::OpenPopup("Select Asset");			

			if (ImGui::BeginDragDropTarget())
			{
				std::string assetTag = "Asset_" + assetRef->GetAssetType();
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(assetTag.c_str());

				if (payload)
				{
					std::string assetPath((char*)payload->Data);

					if (File::DoesFileExist(assetPath))
					{
						AssetHandle handle(assetPath);
						assetRef->SetAssetHandle(handle);
					}
				}

				ImGui::EndDragDropTarget();
			}

			if (ImGui::BeginPopup("Select Asset"))
			{
				for (auto& asset : m_App->GetAssetManager().GetAllDiscorveredAssets())
				{
					if (asset.GetAssetType() == assetRef->GetAssetType())
					{
						std::string ItemText = "(" + asset.GetAssetType() + ") " + asset.GetAssetName();
						if (ImGui::MenuItem(ItemText.c_str()))
						{
							assetRef->SetAssetHandle(asset);
							ImGui::CloseCurrentPopup();
						}
					}
				}

				ImGui::EndPopup();
			}
		}

		void DrawProperty(Property& prop, Ref<ObjectBase> owner = nullptr, bool ForceNewLine = false)
		{
			std::string DisplayName = prop.m_name;
			ImGui::PushID(prop.m_Value);

			if (prop.m_Type == PropType::ARRAY)
			{
				Array<bool>* arrayToDraw = (Array<bool>*)prop.m_Value;				
				ImGui::Text(prop.m_name.c_str());
				ImGui::PushItemWidth(-1);
				ImGui::SameLine();

				ImGui::PushItemWidth(ImGui::CalcItemWidth());
				bool expanded = ImGui::TreeNodeEx((void*)(prop.m_Value), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap, "Array");
				ImGui::PopItemWidth();

				ImGui::SameLine();
				float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight, lineHeight };

				if (owner && ImGui::Button("+", buttonSize))
				{
					ObjectUtils::AddEmptyArrayPropertyElement(arrayToDraw, m_App->GetStructRegistry());
				}
				ImGui::SameLine();
				if (ImGui::Button("<"))
				{
					arrayToDraw->m_InternalArray.clear();
				}
				if (expanded)
				{		
					ListProperties(*(std::vector<Property>*)&arrayToDraw->m_InternalArray, nullptr, true, false);
					ImGui::TreePop();
				}
				ImGui::PopItemWidth();
			}
			else
			if (prop.m_Type == PropType::STRUCT)
			{
				StructBase* structToDraw = (StructBase*)prop.m_Value;
				ImGui::Text(prop.m_name.c_str());
				ImGui::SameLine();

				ImGui::PushItemWidth(ImGui::CalcItemWidth());
				bool expanded = ImGui::TreeNodeEx((void*)(prop.m_Value), ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap, structToDraw->GetClassType().Name.c_str());
				ImGui::PopItemWidth();

				ImGui::SameLine();
				float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight, lineHeight };

				if (owner && ImGui::Button("<", buttonSize))
				{
					ResetProperty(owner.get(), prop.m_name, m_App->GetObjectRegistry());
				}

				if (expanded)
				{
					if(!owner)
						ImGui::NewLine();

					ListProperties(structToDraw->GetPropertiesMutable(), nullptr, true, false);
					ImGui::TreePop();
				}			
			}
			else
			{
				ImGui::Text(DisplayName.c_str());
				ImGui::SameLine(0, 2);

				ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() - 30);

				DisplayName = "";
				StructBase* structToDraw = nullptr;
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
						DrawVec3Control(prop.m_Value, glm::value_ptr(*(vec3d*)prop.m_Value));
						break;
					}
					case PropType::COLOR3:
					{
						ImGui::ColorEdit3(DisplayName.c_str(), glm::value_ptr(*(color3*)prop.m_Value));
						break;
					}
					case PropType::COLOR4:
					{
						ImGui::ColorEdit4(DisplayName.c_str(), glm::value_ptr(*(color4*)prop.m_Value));
						break;
					}
					case PropType::TRANSFORM:
					{
						//draw transform
						ImGui::PushItemWidth(ImGui::CalcItemWidth());
						bool expanded = ImGui::TreeNodeEx(prop.m_Value, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed, "Transform");
						if (expanded)
						{
							Transform* transform = (Transform*)prop.m_Value;
					
							DrawVec3Control("Position", glm::value_ptr(transform->pos));
							DrawVec3Control("Rotation", glm::value_ptr(transform->rot));
							DrawVec3Control("Scale", glm::value_ptr(transform->scale), 1.0f, 0.05f);
							ImGui::TreePop();
						}
						ImGui::PopItemWidth();
						break;
					}

					case PropType::ASSETREF:
					{
						DrawAssetRef((AssetRef<Asset>*)prop.m_Value);
						break;
					}
					
				}
				ImGui::PopItemWidth();

				
				ImGui::PushItemWidth(-1.f);
				float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
				ImVec2 buttonSize = { lineHeight, lineHeight };
				ImGui::SameLine(ImGui::GetContentRegionAvailWidth() + 10);
				
				if (owner && prop.m_Type != PropType::STRUCT && ImGui::Button("<", buttonSize))
				{
					ResetProperty(owner.get(), prop.m_name, m_App->GetObjectRegistry());
				}

				ImGui::PopItemWidth();			
			}
			//exists cuz im dumb and idk how to make structs look good
			if (ForceNewLine)
				ImGui::NewLine();

			ImGui::PopID();
		}


		static void DrawVec3Control(void* ID, float* value, float ResetValue = 0.f, const float& Speed = 1.0f, float columnWidth = 100.f)
		{
			auto BoldFont = ImGui::GetIO().Fonts->Fonts[0];

			ImGui::PushID(ID);
			float lineHeight = ImGui::GetFont()->FontSize + ImGui::GetStyle().FramePadding.y * 2.0f;
			ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

			ImGui::PushMultiItemsWidths(3, (ImGui::CalcItemWidth() - buttonSize.x * 3.f) + buttonSize.x / 3.f);
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0,0 });

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
	Ref<ObjectComponent> m_SelectedComponent;
	Application* m_App;
	float m_ColumnWidth = 100.f;
};