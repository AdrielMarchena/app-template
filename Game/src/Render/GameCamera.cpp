#include "pch.h"
#include "GameCamera.h"

#include "Core/Assert.h"
#include "Debug/Intrumentator.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Game {

	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographicPX(float w, float h, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::OrthographicPX;
		m_OrthographicPX = { w,h };
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;
		RecalculateProjection();
	}

	void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		GAME_CORE_ASSERT(width > 0 && height > 0,"Game Camera Viewport can't have width or height set to 0");
		if(height)
			m_AspectRatio = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::RecalculateProjection()
	{
		GAME_PROFILE_FUNCTION();
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else if (m_ProjectionType == ProjectionType::Orthographic)
		{
			float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicSize * 0.5f;
			float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		}
		else
		{
			float orthoLeft = -m_OrthographicPX.x * m_AspectRatio * 0.5f;
			float orthoRight = m_OrthographicPX.x * m_AspectRatio * 0.5f;
			float orthoBottom = -m_OrthographicPX.y * m_AspectRatio * 0.5f;
			float orthoTop = m_OrthographicPX.y * m_AspectRatio * 0.5f;

			m_Projection = glm::ortho(orthoLeft, orthoRight,
				orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);

			//m_Projection = glm::ortho(0.0f, m_OrthographicPX.x * m_AspectRatio, 0.0f, m_OrthographicPX.y, m_OrthographicNear, m_OrthographicFar);
		}
	}
}
