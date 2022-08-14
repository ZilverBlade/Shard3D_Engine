#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../../events/event.h"

namespace Shard3D {
	class EngineWindow {
	public:
		enum WindowType {
			Windowed = 0,
			Borderless = 1,
			Fullscreen = 2
		};
	private:
		using EventCallbackFunc = std::function<void(Events::Event&)>;
		friend class GraphicsSettings;
	public:
		EngineWindow(int w, int h, std::string name);
		~EngineWindow();
		
		EngineWindow(const EngineWindow&) = delete;
		EngineWindow& operator=(const EngineWindow&) = delete;

		void setEventCallback(const EventCallbackFunc& cbf) { _wndData.eventCallback = cbf; }

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		void setWindowMode(WindowType winType);
	private:
		void initWindow();

		int width;
		int height;

		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
		GLFWmonitor* monitor = nullptr;

		struct wndData {
			EventCallbackFunc eventCallback;
		};

		wndData _wndData;
		
		// CALLBACKS
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void charCallback(GLFWwindow* window, unsigned int c);
		static void mouseBtnCallback(GLFWwindow* window, int button, int action, int mods);
		static void mouseMotionCallback(GLFWwindow* window, double xpos, double ypos);
		static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	};
}