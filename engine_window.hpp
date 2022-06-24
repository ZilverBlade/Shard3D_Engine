#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Shard3D {

	class EngineWindow {
	protected:
		enum WindowMode {
			Windowed = 0,
			Borderless = 1,
			Fullscreen = 2
		};
	public:
		inline static WindowMode windowType = Windowed; // 0 = windowed; 1 = borderless windowed; 2 = fullscreen
		inline static int borderlessFullscreen = false;

		inline static int windowWidth;
		inline static int windowHeight;

		EngineWindow(int w, int h, std::string name);
		~EngineWindow();
		
		EngineWindow(const EngineWindow&) = delete;
		EngineWindow& operator=(const EngineWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		static GLFWwindow* getGLFWwindow() { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		inline void setWindowMode(WindowMode winType);

		void toggleFullscreen();
	private:
		static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
		void initWindow();

		int width;
		int height;

		inline static int windowPosX;
		inline static int windowPosY;

		bool framebufferResized = false;

		std::string windowName;
		inline static GLFWwindow* window;
		inline static GLFWmonitor* monitor = nullptr;
	};
}