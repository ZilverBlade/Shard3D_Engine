#pragma once

#include "event.h"

namespace Shard3D {
	class KeyEvent : public Event {
	public:
		inline int getKeyCode() const { return key_code; }
		virtual int getCategoryFlags() const override { return EventCategoryKeyboard; }
	protected:
		KeyEvent(int _key_code) : key_code(_key_code) {}
		int key_code;
	};

	class KeyDownEvent : public KeyEvent {
	public:
		KeyDownEvent(const int keycode, bool isRepeat = false) : KeyEvent(keycode), repeating(isRepeat) {}

		bool isRepeating() const { return repeating; }

		std::string toString() const override { return std::string("KeyDownEvent: " + std::to_string(key_code) + " (repeating = " + std::to_string(repeating) + ")"); }

		SHARD3D_EVENT_CLASS_TYPE(KeyDown);		
	private:
		bool repeating;
	};
	class KeyReleaseEvent : public KeyEvent {
	public:
		KeyReleaseEvent(const int keycode) : KeyEvent(keycode) {}

		std::string toString() const override { return std::string("KeyReleaseEvent: " + std::to_string(key_code)); }

		SHARD3D_EVENT_CLASS_TYPE(KeyRelease);
	};
	class KeyPressEvent : public KeyEvent {
	public:
		KeyPressEvent(const int keycode) : KeyEvent(keycode) {}

		std::string toString() const override { return std::string("KeyPressEvent: " + static_cast<char>(key_code) + '0'); }

		SHARD3D_EVENT_CLASS_TYPE(KeyPress);
	};
}