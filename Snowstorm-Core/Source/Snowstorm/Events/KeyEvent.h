#pragma once

#include "Event.h"

namespace Snowstorm
{
	class HAZEL_API KeyEvent : public Event // basically an abstract event
	{
	public:
		int GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		explicit KeyEvent(const int keycode)
			: m_KeyCode(keycode)
		{
		}

		int m_KeyCode;
	};

	class HAZEL_API KeyPressedEvent final : public KeyEvent
	{
	public:
		KeyPressedEvent(const int keycode, const int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount)
		{
		}

		int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		int m_RepeatCount;
	};

	class HAZEL_API KeyReleasedEvent final : public KeyEvent
	{
	public:
		explicit KeyReleasedEvent(const int keycode):
			KeyEvent(keycode)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class HAZEL_API KeyTypedEvent final : public KeyEvent
	{
	public:
		explicit KeyTypedEvent(const int keycode)
			: KeyEvent(keycode) 
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};

}
