#include "shard_window.hpp"
#include <stdexcept>
#include "simpleini/simple_ini.h"
namespace shard {

	ShardWindow::ShardWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	ShardWindow::~ShardWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void ShardWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile("settings/engine_settings.ini");

		bool windowresizable = ini.GetBoolValue("WINDOW", "Resizable");

		if (windowresizable == true) {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		}
		else {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}
		

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}
	void ShardWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void ShardWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto shardWindow = reinterpret_cast<ShardWindow*>(glfwGetWindowUserPointer(window));
		shardWindow->framebufferResized = true;
		shardWindow->width = width;
		shardWindow->height = height;
	}

}