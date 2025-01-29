#pragma once

#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class RenderSystem final : public System
	{
	public:
		explicit RenderSystem(const RegistryRef& registry) : System(registry)
		{
		}

		void execute(Timestep ts) override;
	};
}
