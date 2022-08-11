#pragma once

#include "event.h"

namespace Shard3D {
	class MouseButtonEvent : public Event {
	public:
		inline int getButtonCode() const { return button; }
		virtual int getCategoryFlags() const override { return EventCategoryMouse; }
	protected:
		MouseButtonEvent(int _button) : button(_button) {}
		int button;
	};

	class MouseButtonDownEvent : public MouseButtonEvent {
	public:
		MouseButtonDownEvent(const int button) : MouseButtonEvent(button) {}

		std::string toString() const override { return std::string("MouseButtonDownEvent: " + std::to_string(button)); }
		
		SHARD3D_EVENT_CLASS_TYPE(MouseButtonDown);

	};

	class MouseButtonReleaseEvent : public MouseButtonEvent {
	public:
		MouseButtonReleaseEvent(const int button) : MouseButtonEvent(button) {}

		std::string toString() const override { return std::string("MouseButtonReleaseEvent: " + std::to_string(button)); }
		
		SHARD3D_EVENT_CLASS_TYPE(MouseButtonRelease);
	};

	class MouseHoverEvent : public Event {
	public:
		inline float getXPos() const { return x; }
		inline float getYPos() const { return y; }
		virtual int getCategoryFlags() const override { return EventCategoryMouse; }

		MouseHoverEvent(const float _x, const float _y) : x(_x), y(_y) {}

		std::string toString() const override { return std::string("MouseHoverEvent: (" + std::to_string(x) + ", " + std::to_string(y) + ")"); }

		SHARD3D_EVENT_CLASS_TYPE(MouseHover);
	protected:
		float x, y;
	};

	class MouseScrollEvent : public Event {
	public:
		inline float getXOffset() const { return x; }
		inline float getYOffset() const { return y; }
		virtual int getCategoryFlags() const override { return EventCategoryMouse; }

		MouseScrollEvent(const float _x, const float _y) : x(_x), y(_y) {}

		std::string toString() const override { return std::string("MouseScrollEvent: (" + std::to_string(x) + ", " + std::to_string(y) + ")"); }

		SHARD3D_EVENT_CLASS_TYPE(MouseScroll);
	protected:
		float x, y;
	};
}