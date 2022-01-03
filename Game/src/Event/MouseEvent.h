#pragma once

#include "Events.h"
#include <sstream>
	class MouseMovedEvent : public Event
	{
	private:
		float m_MouseX, m_MouseY;
	public:
		MouseMovedEvent(float x, float y)
			:m_MouseX(x),m_MouseY(y){}

		float GetX() const { return m_MouseX; }
		float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_MOVE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)
	};

	class MouseScrollEvent : public Event
	{
	private:
		float m_XOffset, m_YOffset;
	public:
		MouseScrollEvent(float xoffset, float yoffset)
			:m_XOffset(xoffset), m_YOffset(yoffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrollEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_SCROLLED)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)
	};

	class MouseButtonEvent : public Event
	{
	protected:
		int m_Button;
		MouseButtonEvent(int keycode)
			:m_Button(keycode) {}

	public:
		int GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int keycode)
			: MouseButtonEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MOUSE_BUTTON_PRESSED)
	};

	class MouseButtonReleaseEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleaseEvent(int keycode)
			: MouseButtonEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleaseEvent: " << m_Button;
			return ss.str();
		}
		EVENT_CLASS_TYPE(MOUSE_BUTTON_RELEASED)
	};
