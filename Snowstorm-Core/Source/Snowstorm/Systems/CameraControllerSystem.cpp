#include "CameraControllerSystem.hpp"

#include "Snowstorm/Core/Input.h"
#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Events/KeyEvent.h"
#include "Snowstorm/Events/MouseEvent.h"
#include "Snowstorm/Scene/Components.h"

namespace Snowstorm
{
	void CameraControllerSystem::execute(const Timestep ts)
	{
		// Retrieve the EventsHandler singleton
		auto& eventsHandler = singletonView<EventsHandlerSingleton>();

		// Process all MouseScrolledEvents before updating cameras
		for (const auto& event : eventsHandler.process<MouseScrolledEvent>())
		{
			for (const auto cameraControllerView = view<CameraComponent, TransformComponent, CameraControllerComponent>(); const auto entity : cameraControllerView)
			{
				auto [camera, transform, controller] = cameraControllerView.get(entity);

				if (!camera.Primary)
				{
					continue;
				}

				// Adjust zoom based on mouse scroll input
				controller.ZoomLevel -= event->yOffset * 0.25f;
				controller.ZoomLevel = std::max(controller.ZoomLevel, 0.25f);

				if (camera.Camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					camera.Camera.setOrthographicSize(controller.ZoomLevel);
				}
			}
		}

		// Process continuous movement (instead of only on KeyPressedEvent)
		for (const auto cameraControllerView = view<CameraComponent, TransformComponent, CameraControllerComponent>(); const auto entity : cameraControllerView)
		{
			auto [camera, transform, controller] = cameraControllerView.get(entity);

			if (!camera.Primary)
			{
				continue;
			}

			auto moveDir = glm::vec3(0.0f);

			// TODO don't do this if the viewport isn't focused (by adding some other state singleton)
			if (Input::IsKeyPressed(Key::A))
			{
				moveDir.x -= cos(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
				moveDir.y -= sin(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
			}
			if (Input::IsKeyPressed(Key::D))
			{
				moveDir.x += cos(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
				moveDir.y += sin(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
			}
			if (Input::IsKeyPressed(Key::W))
			{
				moveDir.x += -sin(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
				moveDir.y += cos(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
			}
			if (Input::IsKeyPressed(Key::S))
			{
				moveDir.x -= -sin(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
				moveDir.y -= cos(glm::radians(transform.Rotation.z)) * controller.MoveSpeed * ts;
			}

			transform.Position += moveDir;

			if (controller.RotationEnabled)
			{
				float rotation = 0.0f;

				if (Input::IsKeyPressed(Key::Q))
				{
					rotation += controller.RotationSpeed * ts;
				}
				if (Input::IsKeyPressed(Key::E))
				{
					rotation -= controller.RotationSpeed * ts;
				}

				transform.Rotation.z += rotation;
				transform.Rotation.z = glm::mod(transform.Rotation.z + 180.0f, 360.0f) - 180.0f;
			}
		}
	}
}
