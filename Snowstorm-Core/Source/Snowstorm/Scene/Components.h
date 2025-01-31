#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

namespace Snowstorm
{
	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;

		explicit TagComponent(std::string name)
			: Tag(std::move(name))
		{
		}
	};

	struct TransformComponent
	{
		glm::vec3 Position{0.0f, 0.0f, 0.0f};
		glm::vec3 Rotation{0.0f, 0.0f, 0.0f}; // Euler angles in degrees
		glm::vec3 Scale{1.0f, 1.0f, 1.0f};

		TransformComponent() = default;
		~TransformComponent() = default;

		explicit TransformComponent(const glm::vec3& position,
		                            const glm::vec3& rotation = glm::vec3(0.0f),
		                            const glm::vec3& scale = glm::vec3(1.0f))
			: Position(position), Rotation(rotation), Scale(scale)
		{
		}

		TransformComponent(const TransformComponent&) = default;
		TransformComponent(TransformComponent&&) = default;

		TransformComponent& operator=(const TransformComponent&) = default;
		TransformComponent& operator=(TransformComponent&&) = default;

		[[nodiscard]] glm::mat4 getTransformMatrix() const
		{
			glm::mat4 transform = translate(glm::mat4(1.0f), Position);

			// Apply rotations in the correct order: Z (roll) → X (pitch) → Y (yaw)
			transform = rotate(transform, glm::radians(Rotation.z), glm::vec3(0, 0, 1)); // Roll
			transform = rotate(transform, glm::radians(Rotation.x), glm::vec3(1, 0, 0)); // Pitch
			transform = rotate(transform, glm::radians(Rotation.y), glm::vec3(0, 1, 0)); // Yaw

			transform = scale(transform, Scale);
			return transform;
		}

		operator glm::mat4() const { return getTransformMatrix(); }
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};

		SpriteRendererComponent() = default;

		explicit SpriteRendererComponent(const glm::vec4& color) : Color(color)
		{
		}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to scene
		bool FixedAspectRatio = false;

		CameraComponent() = default;
	};

	struct CameraControllerComponent
	{
		bool RotationEnabled = true;
		float ZoomLevel = 10.0f;
		float MoveSpeed = 5.0f;
		float RotationSpeed = 180.0f; // Degrees per second
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		std::function<void()> InstantiateScript;
		std::function<void()> DestroyScript;

		template <typename T>
		void bind()
		{
			InstantiateScript = [this]
			{
				Instance = new T();
			};

			DestroyScript = [this]
			{
				delete Instance;
				Instance = nullptr;
			};
		}
	};
}
