#include <Snowstorm.h>
#include <Snowstorm/Core/EntryPoint.h>

#include "EditorLayer.h"

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

		SnowstormEditor(const SnowstormEditor& other) = delete;
		SnowstormEditor(SnowstormEditor&& other) = delete;
		SnowstormEditor& operator=(const SnowstormEditor& other) = delete;
		SnowstormEditor& operator=(SnowstormEditor&& other) = delete;

		~SnowstormEditor() override
		= default;
	};

	Application* CreateApplication()
	{
		return new SnowstormEditor();
	}
}
