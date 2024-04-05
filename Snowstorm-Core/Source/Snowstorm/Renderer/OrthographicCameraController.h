#pragma once

#include "Snowstorm/Core/Timestep.h"
#include "Snowstorm/Events/ApplicationEvent.h"
#include "Snowstorm/Events/MouseEvent.h"
#include "Snowstorm/Renderer/OrthographicCamera.h"

namespace Snowstorm
{
	struct OrthographicCameraBounds
	{
		float Left, Right;
		float Bottom, Top;

		float GetWidth() const { return Right - Left; }
		float GetHeight() const { return Top - Bottom; }
	};

	class OrthographicCameraController
	{
	public:
		explicit OrthographicCameraController(float aspectRatio, bool rotation = false);

		void OnUpdate(Timestep ts);
		void OnEvent(Event& e);

		void OnResize(float width, float height);

		OrthographicCamera& GetCamera() { return m_Camera; }
		const OrthographicCamera& GetCamera() const { return m_Camera; }

		float GetZoomLevel() const { return m_ZoomLevel; }

		void SetZoomLevel(const float level)
		{
			m_ZoomLevel = level;
			CalculateView();
		}

		const OrthographicCameraBounds& GetBounds() const { return m_Bounds; }

	private:
		void CalculateView();

		bool OnMouseScrolled(const MouseScrolledEvent& e);
		bool OnWindowResized(const WindowResizeEvent& e);

	private:
		float m_AspectRatio;
		float m_ZoomLevel = 1.0f;
		OrthographicCameraBounds m_Bounds;
		OrthographicCamera m_Camera;

		bool m_Rotation;

		glm::vec3 m_CameraPosition = {0.0f, 0.0f, 0.0f};
		float m_CameraRotation = 0.0f;
		float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
	};
}
