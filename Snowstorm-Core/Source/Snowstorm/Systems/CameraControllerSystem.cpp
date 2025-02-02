#include "CameraControllerSystem.hpp"

#include <glm/detail/type_quat.hpp>

#include "Snowstorm/Core/Input.h"
#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Events/MouseEvent.h"
#include "Snowstorm/Scene/Components.hpp"

namespace Snowstorm
{
	void CameraControllerSystem::Execute(const Timestep ts)
	{
		auto& eventsHandler = SingletonView<EventsHandlerSingleton>();

		for (const auto cameraControllerView = View<CameraComponent, TransformComponent, CameraControllerComponent>(); const auto entity : cameraControllerView)
		{
			auto [camera, transform, controller] = cameraControllerView.get(entity);

			if (!camera.Primary) continue;

			bool isPerspective = camera.Camera.getProjectionType() == SceneCamera::ProjectionType::Perspective;
			bool rightClickHeld = Input::IsMouseButtonPressed(Mouse::ButtonRight);

			static bool wasRightClickHeld = false;
			static std::pair<float, float> lastMousePos = Input::GetMousePosition();

			// **Fix abrupt jumps when enabling look mode**
			if (rightClickHeld && !wasRightClickHeld)
			{
				Input::SetCursorMode(CursorMode::Locked);
				lastMousePos = Input::GetMousePosition();
			}
			else if (!rightClickHeld && wasRightClickHeld)
			{
				Input::SetCursorMode(CursorMode::Normal);
			}
			wasRightClickHeld = rightClickHeld;

			glm::vec3 moveDir(0.0f);

			// **Mouse Look in Perspective Mode**
			if (isPerspective && rightClickHeld)
			{
				auto [mouseX, mouseY] = Input::GetMousePosition();
				float deltaX = mouseX - lastMousePos.first;
				float deltaY = lastMousePos.second - mouseY; // Inverted Y-axis

				lastMousePos = {mouseX, mouseY}; // Update last position

				// **Apply correct yaw (Y rotation) and pitch (X rotation)**
				transform.Rotation.y -= deltaX * controller.LookSensitivity;
				transform.Rotation.x += deltaY * controller.LookSensitivity;
			}

			// **Quaternion-based Rotation for Correct 3D Movement**
			auto rotation = glm::quat(glm::vec3(transform.Rotation.x, transform.Rotation.y, 0.0f));
			glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
			glm::vec3 right = normalize(cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));

			// **Correct movement logic in Perspective Mode**
			if (isPerspective && rightClickHeld)
			{
				if (Input::IsKeyPressed(Key::W)) moveDir += forward;
				if (Input::IsKeyPressed(Key::S)) moveDir -= forward;
				if (Input::IsKeyPressed(Key::D)) moveDir += right;
				if (Input::IsKeyPressed(Key::A)) moveDir -= right;
			}
			else
			{
				// **Correct movement logic in Orthographic Mode**
				if (Input::IsKeyPressed(Key::A)) moveDir.x -= 1.0f;
				if (Input::IsKeyPressed(Key::D)) moveDir.x += 1.0f;
				if (Input::IsKeyPressed(Key::W)) moveDir.y += 1.0f;
				if (Input::IsKeyPressed(Key::S)) moveDir.y -= 1.0f;
			}

			transform.Position += moveDir * controller.MoveSpeed * ts.GetSeconds();

			// **Proper Zoom Behavior in Perspective Mode (Move Forward/Backward)**
			for (const auto& event : eventsHandler.process<MouseScrolledEvent>())
			{
				float zoomSpeed = 5.0f;
				transform.Position += forward * event->yOffset * zoomSpeed;
			}
		}
	}
}
