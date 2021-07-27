#pragma once
#include "Core/Core.h"
#include "Rendering/RenderAPI.h"


enum class ProjectionType
{
	ORTHO = 0,
	PERSPECTIVE
};

class DENGINE_API Camera
{
public:
	Camera(Ref<RenderAPI> renderAPI) : m_RenderAPI(renderAPI)
	{

	}

	const Transform& GetTransform() const
	{
		return m_Transform;
	}

	void SetTransform(const Transform& trans)
	{
		m_Transform = trans;
		RecalculateViewMatrix();
	}

	void SetPosition(const vec3d& Position)
	{
		auto trans = GetTransform();
		trans.pos = Position;
		SetTransform(trans);
	}

	void SetRotation(const vec3d& rot)
	{
		auto trans = GetTransform();
		trans.rot = rot;
		SetTransform(trans);
	}

	const glm::vec3& GetRotation() const 
	{
		return GetTransform().rot; 
	}

	const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; };
	const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; };
	const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; };

	const float& GetZoom() const
	{ 
		return m_Zoom;
	}

	void SetZoom(const float& zoom)
	{
		m_Zoom = MAX(zoom, 0.01f);
		RecalculateViewMatrix();
	}

	Ref<RenderAPI> GetRenderAPI() 
	{
		return m_RenderAPI;
	}

	void RecalculateViewMatrix();

	void SetProjectionType(const ProjectionType& proj)
	{
		m_ProjectionType = proj;
		RecalculateViewMatrix();
	}

	const ProjectionType& GetProjectionType() const
	{
		return m_ProjectionType;
	}

	void SetFOV(const float& fov)
	{
		m_FOV = fov;
	}

	const float& getFOV() const
	{
		return m_FOV;
	}

private:
	Ref<RenderAPI> m_RenderAPI;
	ProjectionType m_ProjectionType = ProjectionType::PERSPECTIVE;

	glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
	glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
	glm::mat4 m_ViewProjectionMatrix = glm::mat4(1.0f);

	float m_FOV = 60.f;
	float m_Zoom = 1.0f;
	Transform m_Transform;		
};