#include "pch.h"
#include "Snowstorm/Core/Window.h"

#ifdef SS_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif

namespace Snowstorm
{
	WindowProps::WindowProps(const std::string& title, const uint32_t width, uint32_t height): Title(title),
		Width(width), Height(height)
	{
	}

	Scope<Window> Window::Create(const WindowProps& props)
	{
#ifdef SS_PLATFORM_WINDOWS
		return CreateScope<WindowsWindow>(props);
#else
		SS_CORE_ASSERT(false, "Unknown platform!");
		return nullptr;
#endif
	}
}
