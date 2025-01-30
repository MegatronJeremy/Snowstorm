#pragma once

#include <glm/glm.hpp>

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
		glm::mat4 Transform{1.0f};

		TransformComponent() = default;
		~TransformComponent() = default;

		explicit TransformComponent(const glm::mat4& transform)
			: Transform(transform)
		{
		}

		TransformComponent(const TransformComponent&) = default;
		TransformComponent(TransformComponent&&) = default;

		TransformComponent& operator=(const TransformComponent&) = default;
		TransformComponent& operator=(TransformComponent&&) = default;

		operator glm::mat4&() { return Transform; }
		operator const glm::mat4&() const { return Transform; }
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
