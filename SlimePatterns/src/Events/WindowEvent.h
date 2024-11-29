#pragma once
#include "Event.h"
#include <sstream>

class WindowResizeEvent : public Event
{
public:
	WindowResizeEvent(unsigned int width, unsigned int height)
		: m_Width(width), m_Height(height) {}

	unsigned int GetWidth() const { return m_Width; }
	unsigned int GetHeight() const { return m_Height; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
		return ss.str();
	}

	EVENT_CLASS_TYPE(WindowResize)

private:
	unsigned int m_Width, m_Height;
};

class WindowMovedEvent : public Event
{
public:
	WindowMovedEvent(unsigned int x, unsigned int y)
		: m_PositionX(x), m_PositionY(y) {}

	unsigned int GetPositionX() const { return m_PositionX; }
	unsigned int GetPositionY() const { return m_PositionY; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "WindowMovedEvent: " << m_PositionX << ", " << m_PositionY;
		return ss.str();
	}

	EVENT_CLASS_TYPE(WindowMoved)

private:
	unsigned int m_PositionX, m_PositionY;
};

class ScrollEvent : public Event
{
public:
	ScrollEvent(double xoff, double yoff)
		: m_Xoffset(xoff), m_Yoffset(yoff) {}

	double GetX() const { return m_Xoffset; }
	double GetY() const { return m_Yoffset; }

	std::string ToString() const override
	{
		std::stringstream ss;
		ss << "ScrollEvent: " << m_Xoffset << ", " << m_Yoffset;
		return ss.str();
	}

	EVENT_CLASS_TYPE(Scroll)

private:
	double m_Xoffset, m_Yoffset;
};