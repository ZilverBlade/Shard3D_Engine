#pragma once

#include "event.h"

namespace Shard3D::Events {
	class WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(uint32_t _width, uint32_t _height) : width(_width), height(_height) {}

		uint32_t getWidth() const { return width; }
		uint32_t getHeight() const { return height; }

		std::string toString() const override { return std::string("WindowResizeEvent: {" + std::to_string(width) + ", " + std::to_string(height) + "}"); }

		static Shard3D::Events::EventType getStaticType() { return Shard3D::Events::EventType::WindowResize; }
		virtual Shard3D::Events::EventType getEventType() const override { return getStaticType(); }
		virtual const char* getName() const override { return "EventType::WindowResize"; }
		virtual int getCategoryFlags() const override { return EventCategoryWindow; }
	private:
		uint32_t width, height;
	};
}