#pragma once
#include "s3dtpch.h"
 
namespace Shard3D {
	class EngineWindow {
	public:
		enum WindowType {
			Windowed = 0,
			Borderless = 1,
			Fullscreen = 2
		};
	private:

		friend class GraphicsSettings;
	public:
		EngineWindow(int w, int h, std::string name);
		~EngineWindow();
		
		EngineWindow(const EngineWindow&) = delete;
		EngineWindow& operator=(const EngineWindow&) = delete;

		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { (uint32_t)(width), (uint32_t)(height) }; }
		bool wasWindowResized() { return framebufferResized; }
		void resetWindowResizedFlag() { framebufferResized = false; }
		static GLFWwindow* getGLFWwindow() { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
		void setWindowMode(WindowType winType);
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