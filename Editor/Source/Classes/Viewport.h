#pragma once
#include "DEngine.h"
#include "ImGuizmo.h"

class Viewport
{
	public:
		Viewport(Ref<Scene> scene, Ref<RenderAPI> ParentWindowContext, Ref<Camera> camera, const std::string& viewportName ) : m_Scene(scene), m_RenderAPI(ParentWindowContext), m_ViewportName(viewportName)
		{
			if(m_Scene)
			{ 
				m_Scene->SetRenderAPI(m_RenderAPI);
			}

			FrameBuferSpec spec;
			spec.Width = 1;
			spec.Height = 1;
			m_Framebuffer = MakeRef<FrameBuffer>(spec);

			if(camera)
				m_Camera = camera;
			else
				m_Camera = MakeRef<Camera>(m_RenderAPI);


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
			m_RenderAPI->SetViewport(m_LastViewportSize);
			m_RenderAPI->SetCamera(m_Camera);
			m_RenderAPI->SetActiveFramebuffer(m_Framebuffer);
			m_RenderAPI->SetClearColor({0,0,0,1});
			m_RenderAPI->Clear();	
			HandleCameraMovement(overrideFocus);
		}
		void EndFrame(bool DrawImGui = true)
		{
			m_Framebuffer->Bind();
			m_RenderAPI->SetCamera(m_Camera);

			if(m_Scene)
				m_Scene->RenderFrame(m_Camera);

			m_Framebuffer->Unbind();
			m_RenderAPI->SetActiveFramebuffer(nullptr);

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

			if(DrawImGui)
				ImGui::Begin(m_ViewportName.c_str());

 			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
 			ImVec2 viewportPanelPos = ImGui::GetWindowPos();
 			vec2d ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
 			ViewportSize.x = MAX(ViewportSize.x, 1);
 			ViewportSize.y = MAX(ViewportSize.y, 1);

  			if (m_LastViewportSize.x != ViewportSize.x || m_LastViewportSize.y != ViewportSize.y)
 			{
				m_LastViewportSize = ViewportSize;
 				m_Framebuffer->Resize(ViewportSize.x, ViewportSize.y);
				m_RenderAPI->SetViewport(m_LastViewportSize);
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
								trans = m_Camera->GetTransform();
								trans.pos += World::GetForwardVector(m_Camera->GetTransform().rot) * 300.f;

								SceneUitls::SpawnPrefabInScene(assetRef, m_Scene.get(), trans, ObjectInitializer());
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
								SceneUitls::LoadSceneFromAsset(prefabAsset, m_Scene);
							}
						}
					}

					ImGui::EndDragDropTarget();
				}
			}


			if (DrawImGui)
				ImGui::End();

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
			m_RenderAPI->SetInputMode(InputMode::GAME);
		}

		void ExitFocus()
		{
			m_RenderAPI->SetInputMode(InputMode::UI);
			m_InputManager.ClearInput();
		}

		void RenderGizmo()
		{
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

		
	public:
		std::string m_ViewportName;
		vec2d m_LastViewportSize = {1,1};
		vec2d m_ViewportPos;
		bool m_FocusedOnviewport = false;
		bool m_WasFocusedOnviewport = false;
		bool m_RecieveInput = false;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Scene> m_Scene;
		Ref<RenderAPI> m_RenderAPI;
		Ref<RenderAPI> m_ParentWindowContext;
		Ref<Camera> m_Camera;

		float m_CameraMovementSpeed = 200.f;
		float m_CameraRotationSpeed = 100.f;

		InputManager m_InputManager;
		Callback<MouseEvent> m_MouseCallback;
		vec2d m_LastMouseVector = {0,0};

		//gizmo stuff
		bool m_DrawGizmo = true;
		ObjectComponent* m_SelectedComponent = nullptr;
		ImGuizmo::OPERATION m_TransformMode = ImGuizmo::TRANSLATE;
};


