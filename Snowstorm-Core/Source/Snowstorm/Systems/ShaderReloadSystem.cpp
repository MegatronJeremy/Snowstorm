#include "ShaderReloadSystem.hpp"

#include "Snowstorm/Renderer/Shader.hpp"

namespace Snowstorm
{
	void ShaderReloadSystem::Execute(const Timestep ts)
	{
		static float timeSinceLastCheck = 0.0f;
		timeSinceLastCheck += ts.GetSeconds();

		// Check for updates every 1 second
		if (timeSinceLastCheck > 1.0f)
		{
			auto& shaderLibrary = SingletonView<ShaderLibrarySingleton>();

			shaderLibrary.ReloadAll();
			timeSinceLastCheck = 0.0f;
		}
	}
}
