#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

#include "Snowstorm/Renderer/Framebuffer.h"
#include "Snowstorm/Renderer/Texture.h"

namespace Snowstorm
{
	struct TagComponent
	{
		std::string Tag;
	};

	struct TransformComponent
	{
		glm::vec3 Position{0.0f, 0.0f, 0.0f};
		glm::vec3 Rotation{0.0f, 0.0f, 0.0f}; // Euler angles in degrees
		glm::vec3 Scale{1.0f, 1.0f, 1.0f};

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

	struct ViewportComponent
	{
		glm::vec2 Size = {1280, 720};
		bool Focused = false;
		bool Hovered = false;
	};

	struct FramebufferComponent
	{
		Ref<Framebuffer> Framebuffer;
		bool Active = true; // Enable or disable rendering for this framebuffer
	};

	struct RenderTargetComponent
	{
		entt::entity TargetFramebuffer;
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
		Ref<Texture2D> Texture;
		float TilingFactor = 1.0f;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true; // TODO: think about moving to scene
		bool FixedAspectRatio = false;
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
