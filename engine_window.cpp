#include "engine_window.hpp"
#include <stdexcept>
#include "simpleini/simple_ini.h"
#include "utils/definitions.hpp"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Shard3D {

	EngineWindow::EngineWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	EngineWindow::~EngineWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void EngineWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		if (ini.GetBoolValue("WINDOW", "Resizable")) {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		}
		else {
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		}
		
		window = glfwCreateWindow(ini.GetLongValue("WINDOW", "DEFAULT_WIDTH"), ini.GetLongValue("WINDOW", "DEFAULT_HEIGHT"), ini.GetValue("WINDOW", "WindowName"), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		GLFWimage images[1];
		images[0].pixels = stbi_load(WINDOW_ICON_PATH, &images[0].width, &images[0].height, 0, 4); //rgba channels 
		glfwSetWindowIcon(window, 1, images);
		stbi_image_free(images[0].pixels);

		monitor = glfwGetPrimaryMonitor();

		ini.LoadFile(GAME_SETTINGS_PATH);

		if ((std::string)ini.GetValue("WINDOW", "WINDOW_TYPE") == "Borderless") {
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowSize(window, mode->width, mode->height);
			std::cout << "Set Borderless Fullscreen\n";
			windowType = 1;
		} else if ((std::string)ini.GetValue("WINDOW", "WINDOW_TYPE") == "Fullscreen") {
			// get resolution of monitor
			const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
			// switch to full screen
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
			std::cout << "Set Fullscreen\n";
			windowType = 2;

		} else { //for windowed mode, used as a fallback if invalid options chosen lol
			std::cout << "Set Windowed\n";
			windowType = 0;
		}

	}
	void EngineWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void EngineWindow::toggleFullscreen() {
		if (borderlessFullscreen == false && windowType == 2) {
		
			const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

			// restore last window size and position
			glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowWidth, windowHeight, videoMode->refreshRate);

			windowType = 0;
		}
		else {
			CSimpleIniA ini;
			ini.SetUnicode();
			ini.LoadFile(GAME_SETTINGS_PATH);

			// backup window position and window size
			glfwGetWindowPos(window, &windowPosX, &windowPosY);
			glfwGetWindowSize(window, &windowWidth, &windowHeight);
			ini.SetLongValue("WINDOW", "WIDTH", (long)windowWidth);
			ini.SetLongValue("WINDOW", "HEIGHT", (long)windowHeight);
			// get resolution of monitor
			const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
			// switch to full screen
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
			std::cout << "Set Fullscreen\n";
			windowType = 2;
		
		}
	}


	void EngineWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto engineWindow = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window));
		engineWindow->framebufferResized = true;
		engineWindow->width = width;
		engineWindow->height = height;
	}

}