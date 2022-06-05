#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Shard3D {

	class EngineWindow {

	public:
		int windowType = 0; // 0 = windowed; 1 = borderless windowed; 2 = fullscreen
		int borderlessFullscreen = false;

		int windowWidth;
		int windowHeight;

		EngineWindow(int w, int h, std::string name);
		~EngineWindow();
		
		EngineWindow(const EngineWindow&) = delete;
		EngineWindow& operator=(const EngineWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }


		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		void toggleFullscreen();
	private:
		static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
		void initWindow();

		int width;
		int height;

		int windowPosX;
		int windowPosY;

		bool framebufferResized = false;

		std::string windowName;
		GLFWwindow* window;
		GLFWmonitor* monitor = nullptr;
	};
}