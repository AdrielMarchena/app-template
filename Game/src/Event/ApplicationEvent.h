#pragma once
#include <stdint.h>
#include "Events.h"
#include <sstream>
	class WindowResizeEvent : public Event
	{
	private:
		uint32_t m_Width, m_Height;
	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			:m_Width(width), m_Height(height){ }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WINDOW_RESIZE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_APPLICATION)
	};

	class WindowCloseEvent : public Event
	{
	private:
	public:
		WindowCloseEvent(){}

		EVENT_CLASS_TYPE(WINDOW_CLOSE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_APPLICATION)
	};
