#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "SceneCamera.h"
#include "ScriptableEntity.h"

#include "Snowstorm/Renderer/Framebuffer.hpp"
#include "Snowstorm/Renderer/Material.hpp"
#include "Snowstorm/Renderer/Mesh.hpp"

namespace Snowstorm
{
	// TODO move this to another directory called Components, and separate out everything here

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
		glm::vec2 Size;
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

	struct MaterialComponent
	{
		Ref<Material> MaterialInstance;
	};

	struct MeshComponent
	{
		Ref<Mesh> MeshInstance;
	};

	struct SpriteComponent
	{
		Ref<Texture2D> TextureInstance;
		float TilingFactor = 1.0f;
		glm::vec4 TintColor = glm::vec4{1.0f};

		SpriteComponent(Ref<Texture2D> textureInstance, const float tilingFactor = 1.0f, const glm::vec4& color = glm::vec4{1.0f})
			: TextureInstance(std::move(textureInstance)), TilingFactor(tilingFactor), TintColor(color)
		{
		}

		SpriteComponent(const glm::vec4& color)
			: TintColor(color)
		{
		}
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
		float LookSensitivity = 0.1f; // Degrees per pixel moved
	};

	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		std::function<void()> InstantiateScript;
		std::function<void()> DestroyScript;

		template <typename T>
		void Bind()
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
