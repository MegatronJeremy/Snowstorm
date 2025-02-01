#pragma once

#include <Snowstorm/ECS/System.hpp>

namespace Snowstorm
{
	class ScriptSystem final : public System
	{
	public:
		explicit ScriptSystem(const SceneRef scene) : System(scene)
		{
		}

		void Execute(Timestep ts) override;
	};
}
