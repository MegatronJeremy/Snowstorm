#pragma once

#include "pch.h"

#include "Event.h"

namespace Snowstorm
{
	struct WindowResizeEvent final : Event
	{
		WindowResizeEvent(const uint32_t width, const uint32_t height)
			: m_Width(width), m_Height(height)
		{
			SS_CORE_INFO("Window resize event");
		}

		[[nodiscard]] std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

		uint32_t m_Width, m_Height;
	};

	struct WindowCloseEvent final : Event
	{
		WindowCloseEvent()
		= default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	struct AppTickEvent final : Event
	{
		AppTickEvent()
		= default;

		EVENT_CLASS_TYPE(AppTick)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	struct AppUpdateEvent final : Event
	{
		AppUpdateEvent()
		= default;

		EVENT_CLASS_TYPE(AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};

	struct AppRenderEvent final : Event
	{
		AppRenderEvent()
		= default;

		EVENT_CLASS_TYPE(AppRender)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
