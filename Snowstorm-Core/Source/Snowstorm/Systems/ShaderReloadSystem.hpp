#pragma once
#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class ShaderReloadSystem : public System
	{
	public:
		explicit ShaderReloadSystem(const SceneRef context)
			: System(context)
		{
		}

		void Execute(Timestep ts) override;
	};
}
