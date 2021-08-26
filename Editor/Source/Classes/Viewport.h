#pragma once
#include "DEngine.h"
#include "ImGuizmo.h"

class Viewport
{
	public:
		Viewport(Ref<Scene> scene, Ref<Window> WindowContext, Ref<Camera> camera, const std::string& viewportName ) : m_Scene(scene), m_Window(WindowContext), m_ViewportName(viewportName)
		{
			FrameBuferSpec spec;
			spec.Width = 1;
			spec.Height = 1;
			m_Framebuffer = MakeRef<FrameBuffer>(spec);

			if(camera)
				m_Camera = camera;
			else
				m_Camera = MakeRef<Camera>(WindowContext->GetRenderAPI());


			//mouse callback
			m_MouseCallback.Assign([&](MouseEvent* event)
			{
				if (event->GetEventType() == MouseEventType::MOVED)
				{
					m_LastMouseVector = event->GetMoveVector();
				}
			});

			m_InputManager.BindOnMouseMove(m_MouseCallback);
		}

		~Viewport()
		{
			
		}

		//only used to clear the sceen basically
		void BeginFrame(bool overrideFocus = false)
		{
			m_Framebuffer->Bind();
			m_Window->GetRenderAPI()->SetViewport(m_LastViewportSize);
			m_Window->GetRenderAPI()->SetClearColor({0,0,0,1});
			m_Window->GetRenderAPI()->Clear();
			HandleCameraMovement(overrideFocus);
		}
		void EndFrame(bool DrawImGui = true)
		{
			m_Framebuffer->Bind();

			if(m_Scene && m_Scene->GetPipeline())
				m_Scene->GetPipeline()->RenderFrame(m_Camera);

			m_Framebuffer->Unbind();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
			bool open = true;
			if(DrawImGui)
			{ 
				ImGui::Begin(m_ViewportName.c_str(), &open);
				if (!open)
					m_Close = true;
			}

 			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
 			ImVec2 viewportPanelPos = ImGui::GetWindowPos();
 			vec2d ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
 			ViewportSize.x = MAX(ViewportSize.x, 1);
 			ViewportSize.y = MAX(ViewportSize.y, 1);

  			if (m_LastViewportSize.x != ViewportSize.x || m_LastViewportSize.y != ViewportSize.y)
 			{
				m_LastViewportSize = ViewportSize;
 				m_Framebuffer->Resize(ViewportSize.x, ViewportSize.y);
				m_Window->GetRenderAPI()->SetViewport(m_LastViewportSize);
				m_Camera->RecalculateViewMatrix();		
 			}
 			m_ViewportPos = vec2d(viewportPanelPos.x, viewportPanelPos.y);

 			uint64_t textureID = m_Framebuffer->GetColorAttachement();
 			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_LastViewportSize.x, m_LastViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			bool isFocused = ImGui::IsWindowFocused();
			m_WasFocusedOnviewport = m_FocusedOnviewport;
			m_FocusedOnviewport = isFocused;

			//clear input on unfocus
			if (!m_FocusedOnviewport && m_WasFocusedOnviewport)
			{
				ExitFocus();
			}

			//draw gizmo
			if(!DrawImGui || m_FocusedOnviewport)
				RenderGizmo();

			if (DrawImGui)
			{
				if (ImGui::BeginDragDropTarget())
				{
					std::string PrefabassetTag = "Asset_PrefabAsset";
					std::string SceneassetTag = "Asset_SceneAsset";
					const ImGuiPayload* Prefabpayload = ImGui::AcceptDragDropPayload(PrefabassetTag.c_str());
					const ImGuiPayload* SceneAssetpayload = ImGui::AcceptDragDropPayload(SceneassetTag.c_str());

					if (Prefabpayload)
					{
						std::string assetPath((char*)Prefabpayload->Data);

						if (File::DoesFileExist(assetPath))
						{
							AssetRef<PrefabAsset> assetRef(assetPath);
							if (m_Scene)
							{
								Transform trans;
								trans.pos += m_Camera->GetTransform().pos;
								trans.pos += World::GetForwardVector(m_Camera->GetTransform().rot) * 10.f;

								SceneUtils::SpawnPrefabInScene(assetRef, m_Scene.get(), trans, ObjectInitializer());
							}
						}
					}

					if (SceneAssetpayload)
					{
						std::string assetPath((char*)SceneAssetpayload->Data);

						if (File::DoesFileExist(assetPath))
						{
							Ref<AssetHandle> handle = MakeRef<AssetHandle>(assetPath);

							if (m_Scene)
							{
								Ref<SceneAsset> prefabAsset = m_Scene->GetApplication()->GetAssetManager().LoadAsset<SceneAsset>(handle);
								SceneUtils::LoadSceneFromAsset(prefabAsset, m_Scene);
							}
						}
					}

					ImGui::EndDragDropTarget();
				}
			}

			DrawMenu();

			if (DrawImGui)
			{ 
				ImGui::End();
			}

			ImGui::PopStyleVar();
		}

		void HandleCameraMovement(bool overrideFocus = false)
		{
			if (m_Scene && (m_FocusedOnviewport || overrideFocus) && m_InputManager.IsKeyDown(GLFW_MOUSE_BUTTON_RIGHT))
			{
				EnterFocus();

				auto trans = m_Camera->GetTransform();

				//movement
				if (m_InputManager.IsKeyDown(GLFW_KEY_W))
				{					
					trans.pos += World::GetForwardVector(trans.rot) * m_CameraMovementSpeed * m_Scene->GetLastTick().DeltaTime;					
				}
				if (m_InputManager.IsKeyDown(GLFW_KEY_S))
				{
					trans.pos -= World::GetForwardVector(trans.rot) * m_CameraMovementSpeed * m_Scene->GetLastTick().DeltaTime;
				}
				if (m_InputManager.IsKeyDown(GLFW_KEY_A))
				{
					trans.pos -= World::GetRightVector(trans.rot) * m_CameraMovementSpeed * m_Scene->GetLastTick().DeltaTime;
				}
				if (m_InputManager.IsKeyDown(GLFW_KEY_D))
				{
					trans.pos += World::GetRightVector(trans.rot) * m_CameraMovementSpeed * m_Scene->GetLastTick().DeltaTime;
				}

				//Rotation 
				if (m_InputManager.IsKeyDown(GLFW_KEY_UP))
				{
					trans.rot.x += m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				}
				if (m_InputManager.IsKeyDown(GLFW_KEY_DOWN))
				{
					trans.rot.x -= m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				}
				if (m_InputManager.IsKeyDown(GLFW_KEY_LEFT))
				{
					trans.rot.y += m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				}
				if (m_InputManager.IsKeyDown(GLFW_KEY_RIGHT))
				{
					trans.rot.y -= m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				}

				trans.rot.x += m_LastMouseVector.y * m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				trans.rot.y += -m_LastMouseVector.x * m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				

				//trans.rot.y += m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				//trans.rot.x += m_CameraRotationSpeed * m_Scene->GetLastTick().DeltaTime;
				m_LastMouseVector  = {0,0};
				m_Camera->SetTransform(trans);
			}
			else
			{
				if (m_WasFocusedOnviewport)
				{
					ExitFocus();
				}
			}
		}

		void EnterFocus()
		{
			m_Window->SetInputMode(InputMode::GAME);
		}

		void ExitFocus()
		{
			m_Window->SetInputMode(InputMode::UI);
			m_InputManager.ClearInput();
		}

		void RenderGizmo()
		{
			if (m_SelectedComponent == nullptr && m_SelectedObject)
			{
				if(auto root = m_SelectedObject->GetRootComponent())
					m_SelectedComponent = root.get();
			}

			if (m_SelectedComponent)
			{
				TransformComponent* selectedTransform = Cast<TransformComponent>(m_SelectedComponent);
				if (selectedTransform)
				{
					ImGuizmo::SetOrthographic(m_Camera->GetProjectionType() == ProjectionType::ORTHO);
					ImGuizmo::SetDrawlist();
					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());
					const glm::mat4& proj = m_Camera->GetProjectionMatrix();
					const glm::mat4& view = m_Camera->GetViewMatrix();

					glm::mat4 transform = selectedTransform->GetWorldMatrix();
					glm::mat4 Delta = glm::mat4(1.0);
					float snapValues[3] = { 0.5f, 0.5f, 0.5f };
					ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), m_TransformMode, ImGuizmo::LOCAL, glm::value_ptr(transform), glm::value_ptr(Delta));
					if (ImGuizmo::IsUsing())
					{
						if (selectedTransform->GetParent())
						{
							glm::mat4 WorldTrans = selectedTransform->GetWorldMatrix();
							transform = selectedTransform->GetMatrix() * glm::inverse(WorldTrans) * transform;

							Transform LocalTransform = selectedTransform->GetLocalTransform();
							Transform Result = World::MakeTransform(transform);
							vec3d deltaRotation = Result.rot - LocalTransform.rot;
							LocalTransform.pos = Result.pos;
							LocalTransform.rot += deltaRotation;
							LocalTransform.scale = Result.scale;
							selectedTransform->SetLocalTransform(LocalTransform);
						}
						else
						{
							Transform LocalTransform = selectedTransform->GetLocalTransform();
							Transform Result = World::MakeTransform(transform);
							vec3d deltaRotation = Result.rot - LocalTransform.rot;
							LocalTransform.pos = Result.pos;
							LocalTransform.rot += deltaRotation;
							LocalTransform.scale = Result.scale;
							selectedTransform->SetLocalTransform(LocalTransform);
						}
					}
				}
			}
		}


		void DrawMenu()
		{
			ImGui::SetCursorPos({ m_MenuOffset.x, m_MenuOffset.y });
			std::string modeStr = "Mode : " + std::string(m_ViewportMode == AppState::GAME ? "Game" : "Editor");

			if (ImGui::Button(modeStr.c_str()))
			{
				m_ViewportMode = m_ViewportMode == AppState::GAME ? AppState::EDITOR : AppState::GAME;
			}
		}

		
	public:
		std::string m_ViewportName;
		vec2d m_LastViewportSize = {1,1};
		vec2d m_ViewportPos;
		bool m_FocusedOnviewport = false;
		bool m_WasFocusedOnviewport = false;
		bool m_RecieveInput = false;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Scene> m_Scene;
		Ref<Window> m_Window;
		Ref<Camera> m_Camera;

		float m_CameraMovementSpeed = 200.f;
		float m_CameraRotationSpeed = 100.f;

		bool m_Close = false;

		InputManager m_InputManager;
		Callback<MouseEvent> m_MouseCallback;
		vec2d m_LastMouseVector = {0,0};

		//menu offset
		vec2d m_MenuOffset = {10.f, 30.f};

		AppState m_ViewportMode;

		//gizmo stuff
		bool m_DrawGizmo = true;
		ObjectComponent* m_SelectedComponent = nullptr;
		SceneObject* m_SelectedObject = nullptr;
		ImGuizmo::OPERATION m_TransformMode = ImGuizmo::TRANSLATE;
};


