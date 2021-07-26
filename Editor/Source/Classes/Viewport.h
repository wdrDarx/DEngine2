#pragma once
#include "DEngine.h"

class Viewport
{
	public:
		Viewport(Ref<Scene> scene, Ref<RenderAPI> ParentWindowContext) : m_Scene(scene), m_ParentWindowContext(ParentWindowContext)
		{
			//glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
			m_Context = glfwCreateWindow(1, 1, "", NULL, NULL);

			m_RenderAPI = MakeRef<RenderAPI>(m_Context);
			m_RenderAPI->SetCurrentContext(m_Context);
			glfwSwapInterval(0);
			m_RenderAPI->SetViewport({1,1});
			m_RenderAPI->SetClearColor({1,0,0,0});
			scene->SetRenderAPI(m_RenderAPI);

			FrameBuferSpec spec;
			spec.Width = m_RenderAPI->GetViewportSize().x;
			spec.Height = m_RenderAPI->GetViewportSize().y;

			m_Framebuffer = MakeRef<FrameBuffer>(spec);

			m_ParentWindowContext->SetCurrentContext(m_ParentWindowContext->GetCurrentContext());
		}

		~Viewport()
		{
			glfwDestroyWindow(m_Context);
		}

		void BeginFrame()
		{
			m_RenderAPI->SetCurrentContext(m_Context);
			m_RenderAPI->Clear();
			glBegin(GL_TRIANGLES);

			glColor3f(0, 1, 0);

			glVertex2f(0, 0);
			glVertex2f(1, 1);
			glVertex2f(1, 0);

			glEnd();
			m_Framebuffer->Bind();
			m_RenderAPI->Clear();
			glBegin(GL_TRIANGLES);

			glColor3f(0, 1, 0);

			glVertex2f(0, 0);
			glVertex2f(1, 1);
			glVertex2f(1, 0);

			glEnd();
			//m_Scene->BeginFrame();	
			m_ParentWindowContext->SetCurrentContext(m_ParentWindowContext->GetCurrentContext());
		}
		void EndFrame()
		{
			
			m_RenderAPI->SetCurrentContext(m_Context);
			//m_Scene->EndFrame();
			//m_Framebuffer->Unbind();
			//m_ParentWindowContext->SetCurrentContext(m_ParentWindowContext->GetCurrentContext());

			

			ImGui::Begin("Viewport");
 			ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
 			ImVec2 viewportPanelPos = ImGui::GetWindowPos();
 			vec2d ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
 			ViewportSize.x = MAX(ViewportSize.x, 1);
 			ViewportSize.y = MAX(ViewportSize.y, 1);

  			if (m_LastViewportSize.x != ViewportSize.x || m_LastViewportSize.y != ViewportSize.y)
 			{
				//m_RenderAPI->SetCurrentContext(m_Context);
 				glfwSetWindowSize(m_Context, ViewportSize.x, ViewportSize.y);
				//m_Framebuffer->Bind();
 				m_Framebuffer->Resize(ViewportSize.x, ViewportSize.y);
				//m_Framebuffer->Unbind();
			//	m_ParentWindowContext->SetCurrentContext(m_ParentWindowContext->GetCurrentContext());
 				//m_MainCamera->RecalculateViewMatrix();
 				m_LastViewportSize = ViewportSize;
 			}
 			m_ViewportPos = vec2d(viewportPanelPos.x, viewportPanelPos.y);
			
			//m_RenderAPI->SetCurrentContext(m_Context);
			if (glfwGetCurrentContext() == m_Context)
			{
				int i = 0;
			}
			if(glfwGetCurrentContext() == m_ParentWindowContext->GetCurrentContext())
			{
				int i = 0;
			}
 			uint64_t textureID = m_Framebuffer->GetColorAttachement();
			//m_ParentWindowContext->SetCurrentContext(m_ParentWindowContext->GetCurrentContext());

 			ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_LastViewportSize.x, m_LastViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
 			m_FocusedOnviewport = ImGui::IsWindowHovered() && ImGui::IsWindowFocused();
			ImGui::End();

			
			m_Framebuffer->Unbind();
			//m_RenderAPI->SetCurrentContext(m_Context);
			glfwSwapBuffers(m_Context);
			glfwPollEvents();
			m_ParentWindowContext->SetCurrentContext(m_ParentWindowContext->GetCurrentContext());
		}
		
	public:
		vec2d m_LastViewportSize = {0,0};
		vec2d m_ViewportPos;
		bool m_FocusedOnviewport;
		Ref<FrameBuffer> m_Framebuffer;
		Ref<Scene> m_Scene;
		Ref<RenderAPI> m_RenderAPI;
		Ref<RenderAPI> m_ParentWindowContext;
		GLFWwindow* m_Context;

		//Ref<Camera> m_Camera;
};


