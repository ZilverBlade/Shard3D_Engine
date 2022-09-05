#pragma once

#include "../utils/engine_utils.h"

#include <functional>
namespace Shard3D {
	class EngineWindow;
}
namespace Shard3D::Events {

	enum class EventType {
		Null = 0,
		KeyPress, KeyDown, KeyRelease,
		MouseClick, MouseButtonDown, MouseButtonRelease, MouseHover, MouseScroll,
		WindowResize
	};

	enum EventCategory {
		EventCategoryKeyboard = BIT(0),
		EventCategoryMouse = BIT(1),
		EventCategoryWindow = BIT(2),
	};
	
	class Event {
	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }
		inline bool isInCategory(EventCategory cat) { return getCategoryFlags() & cat; }
	protected:
		bool handled = false;
		friend class EventDispatcher;
	};

	class EventDispatcher {
		template<typename T>
		using EventFn = std::function<bool(T&)>;
	public:
		EventDispatcher(Event& _event) : event(_event) {}

		template<typename T>
		bool dispatch(EventFn<T> func) {
			if (event.getEventType() == T::getStaticType()) {
				event.handled = func(*(T*)&event);
				return true;
			}
			return false;
		}

	private:
		Event& event;
	};
}
// macros

#define SHARD3D_EVENT_CLASS_TYPE(enumObj)	static Shard3D::Events::EventType getStaticType() { return Shard3D::Events::EventType::enumObj; }\
											virtual Shard3D::Events::EventType getEventType() const override { return getStaticType(); }\
											virtual const char* getName() const override { return "EventType::"#enumObj; }

#define SHARD3D_EVENT_BIND_VOID(func) std::bind(&func, this, std::placeholders::_1)

#define SHARD3D_EVENT_BIND_HANDLER_RFC(wndptr, func) wndptr.setEventCallback(std::bind(&func, this, std::placeholders::_1))
#define SHARD3D_EVENT_BIND_HANDLER_PTR(wndptr, func) wndptr->setEventCallback(std::bind(&func, this, std::placeholders::_1))

#define SHARD3D_EVENT_CREATE_DISPATCHER(event) Shard3D::Events::EventDispatcher dispatcher(event)
#define SHARD3D_EVENT_DISPATCH(type, func) dispatcher.dispatch<type>(std::bind(&func, this, std::placeholders::_1));

