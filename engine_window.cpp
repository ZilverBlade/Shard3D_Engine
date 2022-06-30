#include "engine_window.hpp"
#include <stdexcept>
#include "simpleini/simple_ini.h"
#include "utils/definitions.hpp"
#include <iostream>


#include "engine_logger.hpp"
#include "cheat_codes.hpp"

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

		monitor = glfwGetPrimaryMonitor();
		window = glfwCreateWindow(
			ini.GetLongValue("WINDOW", "DEFAULT_WIDTH"), 
			ini.GetLongValue("WINDOW", "DEFAULT_HEIGHT"), 
			ini.GetValue("WINDOW", "WindowName"), 
			nullptr, 
			nullptr
		);

		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

		Shard3D::CheatCodes::init(window);

	//	GLFWimage images[1];
	//	images[0].pixels = stbi_load(WINDOW_ICON_PATH, &images[0].width, &images[0].height, 0, 4); //rgba channels 
	//	glfwSetWindowIcon(window, 1, images);
	//	stbi_image_free(images[0].pixels);

		ini.LoadFile(GAME_SETTINGS_PATH);

		if ((std::string)ini.GetValue("WINDOW", "WINDOW_TYPE") == "Borderless") {
			setWindowMode(Borderless);
		} else if ((std::string)ini.GetValue("WINDOW", "WINDOW_TYPE") == "Fullscreen") {
			setWindowMode(Fullscreen);
		} else { //for windowed mode, used as a fallback if invalid options chosen lol
			//setWindowMode(Windowed);
		}
	}
	void EngineWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			SHARD3D_FATAL("Failed to create window surface!");
		}
	}

	void EngineWindow::setWindowMode(WindowMode winMode) {
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(GAME_SETTINGS_PATH);

		if (winMode == Windowed){
			glfwDestroyWindow(window);
			glfwWindowHint(GLFW_DECORATED, true);
			window = glfwCreateWindow(
				ini.GetLongValue("WINDOW", "DEFAULT_WIDTH"),
				ini.GetLongValue("WINDOW", "DEFAULT_HEIGHT"),
				ini.GetValue("WINDOW", "WindowName"),
				nullptr,
				nullptr
			);
			SHARD3D_INFO("Set Windowed");
			windowType = Windowed;
			return;
		} else if (winMode == Borderless) {
			glfwDestroyWindow(window);
			glfwWindowHint(GLFW_DECORATED, false);
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			window = glfwCreateWindow(
				mode->width,
				mode->height + 1,
				ini.GetValue("WINDOW", "WindowName"),
				nullptr,
				nullptr
			);
			glfwSetWindowPos(window, 0, 0);
			SHARD3D_INFO("Set Borderless Fullscreen");
			windowType = Borderless;
		} else if (winMode == Fullscreen) {
			// get resolution of monitor
			const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
			// switch to full screen
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
			SHARD3D_INFO("Set Fullscreen");
			windowType = Fullscreen;
		}
	}

	void EngineWindow::toggleFullscreen() {
		if (borderlessFullscreen == false && windowType == 2) {
			setWindowMode(Windowed);
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
			ini.SaveFile(GAME_SETTINGS_PATH);

			setWindowMode(Borderless);
		}
	}


	void EngineWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto engineWindow = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window));
		engineWindow->framebufferResized = true;
		engineWindow->width = width;
		engineWindow->height = height;
	}
}