#pragma once

#include "Snowstorm/Renderer/Camera.hpp"

namespace Snowstorm
{
	// TODO move everything here to CameraComponent

	class SceneCamera final : public Camera
	{
	public:
		enum class ProjectionType : uint8_t { Perspective = 0, Orthographic = 1 };

		SceneCamera();
		~SceneCamera() override = default;

		void SetOrthographic(float size, float nearClip, float farClip);
		void SetPerspective(float verticalFOV, float nearClip, float farClip);

		void SetViewportSize(uint32_t width, uint32_t height);

		float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }

		void SetPerspectiveVerticalFOV(const float verticalFOV)
		{
			m_PerspectiveFOV = verticalFOV;
			RecalculateProjection();
		}

		float GetPerspectiveNearClip() const { return m_PerspectiveNear; }

		void SetPerspectiveNearClip(const float nearClip)
		{
			m_PerspectiveNear = nearClip;
			RecalculateProjection();
		}

		float GetPerspectiveFarClip() const { return m_PerspectiveFar; }

		void SetPerspectiveFarClip(const float farClip)
		{
			m_PerspectiveFar = farClip;
			RecalculateProjection();
		}

		[[nodiscard]] float GetOrthographicSize() const { return m_OrthographicSize; }

		void SetOrthographicSize(const float size)
		{
			m_OrthographicSize = size;
			RecalculateProjection();
		}

		[[nodiscard]] float GetOrthographicNearClip() const { return m_OrthographicNear; }

		void SetOrthographicNearClip(const float nearClip)
		{
			m_OrthographicNear = nearClip;
			RecalculateProjection();
		}

		[[nodiscard]] float GetOrthographicFarClip() const { return m_OrthographicFar; }

		void SetOrthographicFarClip(const float farClip)
		{
			m_OrthographicFar = farClip;
			RecalculateProjection();
		}

		[[nodiscard]] ProjectionType GetProjectionType() const { return m_ProjectionType; }

		void SetProjectionType(const ProjectionType type)
		{
			m_ProjectionType = type;
			RecalculateProjection();
		}

	private:
		void RecalculateProjection();

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PerspectiveFOV = glm::radians(45.0f);
		float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 500.0f;

		float m_OrthographicSize = 10.0f;
		float m_OrthographicNear = -10.0f, m_OrthographicFar = 10.0f;

		float m_AspectRatio = 0.0f;
	};
}
