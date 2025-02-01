#include <Snowstorm.h>
#include <Snowstorm/Core/EntryPoint.h>

#include "EditorLayer.hpp"

namespace Snowstorm
{
	class SnowstormEditor final : public Application
	{
	public:
		SnowstormEditor()
			: Application("Snowstorm-Editor")
		{
			PushLayer(new EditorLayer());
		}
	};

	Application* CreateApplication()
	{
		return new SnowstormEditor();
	}
}
