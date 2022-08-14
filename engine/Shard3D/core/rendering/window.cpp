#include "../../s3dpch.h"  
#include "window.h"
#include "../../utils/simple_ini.h"

#include "../misc/cheat_codes.h"
#include "../asset/texture.h"
#include "../../events/input.h"

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
		glfwSetKeyCallback(window, keyCallback);
		glfwSetCharCallback(window, charCallback);
		glfwSetMouseButtonCallback(window, mouseBtnCallback);
		glfwSetCursorPosCallback(window, mouseMotionCallback);
		glfwSetScrollCallback(window, mouseScrollCallback);

		Shard3D::CheatCodes::init(window);

		GLFWimage images[1];
		images[0].pixels = EngineTexture::getSTBImage(ENSET_WINDOW_ICON_PATH, &images[0].width, &images[0].height, 0, 4); //rgba channels 
		glfwSetWindowIcon(window, 1, images);
		EngineTexture::freeSTBImage(images[0].pixels);

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

	void EngineWindow::setWindowMode(WindowType winType) {
		CSimpleIniA ini;
		ini.LoadFile(GAME_SETTINGS_PATH);

		if (winType == Windowed) {
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
		}
		else if (winType == Borderless) {
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
		}
		else if (winType == Fullscreen) {
			// get resolution of monitor
			const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
			// switch to full screen
			glfwSetWindowMonitor(window, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
			SHARD3D_INFO("Set Fullscreen");
		}
	}

	void EngineWindow::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		wndData& data = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window))->_wndData;
		if (action == GLFW_PRESS) {
			Events::KeyDownEvent _event(key, 0);
			data.eventCallback(_event);
		} else if (action == GLFW_RELEASE){
			Events::KeyReleaseEvent _event(key);
			data.eventCallback(_event);
		} else if (action == GLFW_REPEAT) {
			Events::KeyDownEvent _event(key, true);
			data.eventCallback(_event);
		}
	}
	void EngineWindow::charCallback(GLFWwindow* window, unsigned int c) {
		wndData& data = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window))->_wndData;
		Events::KeyPressEvent _event(c);
		data.eventCallback(_event);
	}
	void EngineWindow::mouseBtnCallback(GLFWwindow* window, int button, int action, int mods) {
		wndData& data = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window))->_wndData;
		if (action == GLFW_PRESS) {
			Events::MouseButtonDownEvent _event(button);
			data.eventCallback(_event);
		}
		else if (action == GLFW_RELEASE) {
			Events::MouseButtonReleaseEvent _event(button);
			data.eventCallback(_event);
		}
	}

	void EngineWindow::mouseMotionCallback(GLFWwindow* window, double xpos, double ypos) {
		wndData& data = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window))->_wndData;
		Events::MouseHoverEvent _event(static_cast<float>(xpos), static_cast<float>(ypos));
		data.eventCallback(_event);
	}

	void EngineWindow::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		wndData& data = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window))->_wndData;
		Events::MouseScrollEvent _event(static_cast<float>(xoffset), static_cast<float>(yoffset));
		data.eventCallback(_event);
	}

	void EngineWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto engineWindow = reinterpret_cast<EngineWindow*>(glfwGetWindowUserPointer(window));
		engineWindow->framebufferResized = true;
		engineWindow->width = width;
		engineWindow->height = height;
	}
}