#pragma once

#include "key_event.h"
#include "mouse_event.h"
#include "singleton.hpp"
namespace Shard3D {
	class Input {
	public:
		static inline bool isKeyDown(int keyCode) { return glfwGetKey(Singleton::engineWindow.getGLFWwindow(), keyCode) == (GLFW_PRESS || GLFW_REPEAT); }
		static inline bool isMouseButtonDown(int button) { return glfwGetMouseButton(Singleton::engineWindow.getGLFWwindow(), button) == GLFW_PRESS; }
	};
}