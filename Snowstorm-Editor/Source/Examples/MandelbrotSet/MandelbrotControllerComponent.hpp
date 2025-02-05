#pragma once

#include <glm/vec2.hpp>

#include "MandelbrotMaterial.hpp"

namespace Snowstorm
{
	struct MandelbrotControllerComponent
	{
		Ref<MandelbrotMaterial> MaterialInstance;
		glm::vec2 Center{-1.25066f, 0.02012f};
		float Zoom{4.0f};
		int MaxIterations{1000};

		MandelbrotControllerComponent() = default;

		explicit MandelbrotControllerComponent(Ref<MandelbrotMaterial> material) : MaterialInstance(std::move(material))
		{
		}
	};
}
