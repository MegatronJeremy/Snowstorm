#pragma once

#include "Snowstorm/Renderer/Material.hpp"

namespace Snowstorm
{
	class MandelbrotMaterial final : public Material
	{
	public:
		explicit MandelbrotMaterial(Ref<Shader> shader)
			: Material(std::move(shader))
		{
			SetCenter({-0.75f, 0.0f});
			SetZoom(4.0f);
			SetMaxIterations(100);
		}

		void SetCenter(const glm::vec2& center) { SetUniform("u_Center", center); }
		void SetZoom(const float zoom) { SetUniform("u_Zoom", zoom); }
		void SetMaxIterations(const int iterations) { SetUniform("u_MaxIterations", iterations); }
	};
}
