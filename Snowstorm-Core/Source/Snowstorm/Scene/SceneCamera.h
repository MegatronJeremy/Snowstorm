#pragma once

#include "Snowstorm/Renderer/Camera.hpp"

namespace Snowstorm
{
	class SceneCamera final : public Camera
	{
	public:
		enum class ProjectionType : uint8_t { Perspective = 0, Orthographic = 1 };

		SceneCamera();
		~SceneCamera() override = default;

		void setOrthographic(float size, float nearClip, float farClip);
		void setPerspective(float verticalFOV, float nearClip, float farClip);

		void setViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }

		void SetPerspectiveVerticalFOV(const float verticalFOV)
		{
			m_PerspectiveFOV = verticalFOV;
			recalculateProjection();
		}

		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }

		void SetPerspectiveNearClip(const float nearClip)
		{
			m_PerspectiveNear = nearClip;
			recalculateProjection();
		}

		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

		void SetPerspectiveFarClip(const float farClip)
		{
			m_PerspectiveFar = farClip;
			recalculateProjection();
		}

		[[nodiscard]] float getOrthographicSize() const { return m_OrthographicSize; }

		void setOrthographicSize(const float size)
		{
			m_OrthographicSize = size;
			recalculateProjection();
		}

		[[nodiscard]] float getOrthographicNearClip() const { return m_OrthographicNear; }

		void setOrthographicNearClip(const float nearClip)
		{
			m_OrthographicNear = nearClip;
			recalculateProjection();
		}

		[[nodiscard]] float getOrthographicFarClip() const { return m_OrthographicFar; }

		void setOrthographicFarClip(const float farClip)
		{
			m_OrthographicFar = farClip;
			recalculateProjection();
		}

		[[nodiscard]] ProjectionType getProjectionType() const { return m_ProjectionType; }

		void setProjectionType(const ProjectionType type)
		{
			m_ProjectionType = type;
			recalculateProjection();
		}

	private:
		void recalculateProjection();

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}
