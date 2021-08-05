#pragma once
#include "DEngine.h"

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
		}

		~Viewport()
		{
			
		}

		//only used to clear the sceen basically
		void BeginFrame()
		{
			m_Framebuffer->Bind();
			m_RenderAPI->SetViewport(m_LastViewportSize);
			m_RenderAPI->SetCamera(m_Camera);
			m_RenderAPI->SetClearColor({0,0,0,1});
			m_RenderAPI->Clear();		

			HandleCameraMovement();
		}
		void EndFrame(bool DrawImGui = true)
		{
			m_Framebuffer->Bind();
			m_RenderAPI->SetCamera(m_Camera);

			if(m_Scene)
				m_Scene->RenderFrame(m_Camera);

			m_Framebuffer->Unbind();

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
 			m_FocusedOnviewport = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();		

			if (DrawImGui)
				ImGui::End();

			ImGui::PopStyleVar();
		}

		void HandleCameraMovement()
		{
			if (m_Scene && m_FocusedOnviewport)
			{
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

				m_Camera->SetTransform(trans);
			}
		}
		
	public:
		std::string m_ViewportName;
		vec2d m_LastViewportSize = {1,1};
		vec2d m_ViewportPos;
		bool m_FocusedOnviewport;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Scene> m_Scene;
		Ref<RenderAPI> m_RenderAPI;
		Ref<RenderAPI> m_ParentWindowContext;
		Ref<Camera> m_Camera;

		float m_CameraMovementSpeed = 100.f;
		float m_CameraRotationSpeed = 90.f;

		InputManager m_InputManager;
};


