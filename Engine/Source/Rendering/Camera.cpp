#include "Camera.h"
#include "DEngine.h"

void Camera::RecalculateViewProjectionMatrix(bool CalcView /*= true*/)
{
	if(CalcView)
	{
		glm::mat4 transform = World::MakeMatrix(GetTransform());
		m_ViewMatrix = glm::inverse(transform);
	}

	float NewWidth = (float)GetRenderAPI()->GetViewportSize().x * m_Zoom;/// WidthRatio;
	float NewHeight = (float)GetRenderAPI()->GetViewportSize().y * m_Zoom; /// HeightRatio;


	if (m_ProjectionType == ProjectionType::ORTHO)
		m_ProjectionMatrix = glm::ortho(-NewWidth, NewWidth, -NewHeight, NewHeight, -1.0f, 1.0f);
	else
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), NewWidth / NewHeight, 0.01f, 10000.f);
	}


	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}
