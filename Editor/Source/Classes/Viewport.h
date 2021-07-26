#pragma once
#include "DEngine.h"

class Viewport
{
	public:
		Viewport(Ref<Scene> scene, Ref<RenderAPI> ParentWindowContext, Ref<Camera> camera, const std::string& viewportName ) : m_Scene(scene), m_RenderAPI(ParentWindowContext), m_ViewportName(viewportName)
		{
			scene->SetRenderAPI(m_RenderAPI);

			FrameBuferSpec spec;
			spec.Width = m_RenderAPI->GetViewportSize().x;
			spec.Height = m_RenderAPI->GetViewportSize().y;
			m_Framebuffer = MakeRef<FrameBuffer>(spec);

			if(camera)
				m_Camera = camera;
			else
				m_Camera = MakeRef<Camera>(m_RenderAPI);
		}

		~Viewport()
		{
			
		}

		void BeginFrame()
		{
			m_Framebuffer->Bind();
			m_RenderAPI->SetViewport({m_Framebuffer->GetSpec().Width, m_Framebuffer->GetSpec().Height});
			m_RenderAPI->SetCamera(m_Camera);
			m_RenderAPI->SetClearColor({0.1,0.2,0,1});
			m_RenderAPI->Clear();

			m_Scene->BeginFrame();	
		}
		void EndFrame()
		{
			m_Framebuffer->Bind();
			m_RenderAPI->SetViewport({ m_Framebuffer->GetSpec().Width, m_Framebuffer->GetSpec().Height });
			m_RenderAPI->SetCamera(m_Camera);

			m_Scene->EndFrame();
			m_Framebuffer->Unbind();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
			ImGui::Begin(m_ViewportName.c_str());
 			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
 			ImVec2 viewportPanelPos = ImGui::GetWindowPos();
 			vec2d ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
 			ViewportSize.x = MAX(ViewportSize.x, 1);
 			ViewportSize.y = MAX(ViewportSize.y, 1);

  			if (m_LastViewportSize.x != ViewportSize.x || m_LastViewportSize.y != ViewportSize.y)
 			{
 				m_Framebuffer->Resize(ViewportSize.x, ViewportSize.y);
				m_Camera->RecalculateViewMatrix();
 				m_LastViewportSize = ViewportSize;
 			}
 			m_ViewportPos = vec2d(viewportPanelPos.x, viewportPanelPos.y);

 			uint64_t textureID = m_Framebuffer->GetColorAttachement();
 			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_LastViewportSize.x, m_LastViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
 			m_FocusedOnviewport = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();
			ImGui::End();
			ImGui::PopStyleVar();
		}
		
	public:
		std::string m_ViewportName;
		vec2d m_LastViewportSize = {0,0};
		vec2d m_ViewportPos;
		bool m_FocusedOnviewport;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Scene> m_Scene;
		Ref<RenderAPI> m_RenderAPI;
		Ref<RenderAPI> m_ParentWindowContext;
		Ref<Camera> m_Camera;
};


