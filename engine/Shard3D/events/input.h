#pragma once

#include "key_event.h"
#include "mouse_event.h"
#include "../core/rendering/window.h"
namespace Shard3D {
	class Input {
	public:
		static void setWindow(EngineWindow& wnd) { wndPtr = &wnd; };
		static inline bool isKeyDown(int keyCode) { return glfwGetKey(wndPtr->getGLFWwindow(), keyCode) == (GLFW_PRESS || GLFW_REPEAT); }
		static inline bool isMouseButtonDown(int button) { return glfwGetMouseButton(wndPtr->getGLFWwindow(), button) == GLFW_PRESS; }
	private:
		static inline EngineWindow* wndPtr;
	};
}