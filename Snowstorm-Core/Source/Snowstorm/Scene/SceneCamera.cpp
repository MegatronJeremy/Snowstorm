#include "pch.h"
#include "SceneCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Snowstorm
{
	SceneCamera::SceneCamera()
	{
		recalculateProjection();
	}

	void SceneCamera::setOrthographic(const float size, const float nearClip, const float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicSize = size;
		m_OrthographicNear = nearClip;
		m_OrthographicFar = farClip;

		recalculateProjection();
	}

	void SceneCamera::setPerspective(const float verticalFOV, const float nearClip, const float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = verticalFOV;
		m_PerspectiveNear = nearClip;
		m_PerspectiveFar = farClip;

		recalculateProjection();
	}

	void SceneCamera::setViewportSize(const uint32_t width, const uint32_t height)
	{
		m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);

		recalculateProjection();
	}

	void SceneCamera::recalculateProjection()
	{
		if (m_ProjectionType == ProjectionType::Perspective)
		{
			m_Projection = glm::perspective(m_PerspectiveFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
		}
		else
		{
			const float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
			const float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
			const float orthoBottom = -m_OrthographicSize * 0.5f;
			const float orthoTop = m_OrthographicSize * 0.5f;

			m_Projection = glm::ortho(
				orthoLeft, orthoRight,
				orthoBottom, orthoTop,
				m_OrthographicNear, m_OrthographicFar);
		}
	}
}
