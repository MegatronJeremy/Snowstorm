#pragma once

#include "Event.h"

namespace Snowstorm
{
	class HAZEL_API MouseMovedEvent final : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: m_MouseX(x), m_MouseY(y)
		{
		}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_MouseX, m_MouseY;
	};

	class HAZEL_API MouseScrolledEvent final : public Event
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		float GetXOffset() const { return m_XOffset; };
		float GetYOffset() const { return m_YOffset; };

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_XOffset, m_YOffset;
	};

	class HAZEL_API MouseButtonEvent : public Event
	{
	public:
		int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	protected:
		explicit MouseButtonEvent(const int button)
			: m_Button(button)
		{
		}

		int m_Button;
	};

	class HAZEL_API MouseButtonPressedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonPressedEvent(const int button)
			: MouseButtonEvent(button)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class HAZEL_API MouseButtonReleasedEvent final : public MouseButtonEvent
	{
	public:
		explicit MouseButtonReleasedEvent(const int button)
			: MouseButtonEvent(button)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}
