#include "../../s3dpch.h" 
#include "graphics_settings.h"
#include "../../core.h"
#include <fstream>
namespace Shard3D {
	void GraphicsSettings::init(EngineWindow* window) {
		// might as well check engine settings here, even tho it's prob too late lol
		std::ifstream infile(ENGINE_SETTINGS_PATH);
		if (infile.good() == false) SHARD3D_FATAL("Critical error! Engine settings config file not found!");
		infile.clear(); infile.close();
		std::ifstream infile2(GAME_SETTINGS_PATH);
		if (infile2.good() == false) SHARD3D_FATAL("Critical error! Game settings config file not found!");
		infile2.clear(); infile2.close();

		engineWindow = window;
		ini.SetUnicode();
		ini.LoadFile(ENGINE_SETTINGS_PATH);

		constInfo.gameName = ini.GetValue("GAME", "gameName");

		ini.LoadFile(GAME_SETTINGS_PATH);
		read();
	}
	void GraphicsSettings::read() {
		if ((std::string)ini.GetValue("WINDOW", "WINDOW_TYPE") == "Borderless")
			graphics.WindowType = Borderless;
		else if ((std::string)ini.GetValue("WINDOW", "WINDOW_TYPE") == "Fullscreen")
			graphics.WindowType = Fullscreen;
		else //for windowed mode, used as a fallback if invalid options chosen lol
			graphics.WindowType = Windowed;
		graphics.WindowWidth = ini.GetLongValue("WINDOW", "WIDTH");
		graphics.WindowHeight = ini.GetLongValue("WINDOW", "HEIGHT");
	
		graphics.VSync = ini.GetBoolValue("DISPLAY", "V-Sync");
	
		graphics.maxAnisotropy = ini.GetBoolValue("TEXTURES", "maxAnisotropy");
		if (graphics.maxAnisotropy < 1 || graphics.maxAnisotropy > 16)
			SHARD3D_FATAL("maxAnisotropy read is invalid!!");
	
		graphics.FramerateCap = ini.GetLongValue("GRAPHICS", "FramerateCap");
		if (graphics.FramerateCap > 0 && !graphics.VSync)
			const_cast<float&>(graphics.FramerateCapInterval) = 1.f / static_cast<float>(graphics.FramerateCap);
		else 
			const_cast<float&>(graphics.FramerateCapInterval) = 0.f;
		if (graphics.FramerateCap < 15 && graphics.FramerateCap > 0 && !graphics.VSync)
			SHARD3D_WARN("Framerate is limited to an extremely low value ({0})! Experience may be worse", graphics.FramerateCap);

		//graphics.MSAASamples = static_cast<VkSampleCountFlagBits>(ini.GetLongValue("GRAPHICS", "MSAASamples"));
		//if (graphics.MSAASamples < 1)
		//	SHARD3D_FATAL("MSAASamples read is invalid!!");
		// let device class decide
	
		graphics.LODCoef = (GraphicsEnum)ini.GetLongValue("GRAPHICS", "LODCoef");
		graphics.ShadowQuality = (GraphicsEnum)ini.GetLongValue("GRAPHICS", "ShadowQuality");
		graphics.ReflectionQuality = (GraphicsEnum)ini.GetLongValue("GRAPHICS", "ReflectionQuality");
	}
	Settings& GraphicsSettings::get() {
		return graphics;
	}
	void GraphicsSettings::set(Settings sets) {
		if (graphics.WindowType == Borderless)
			ini.SetValue("WINDOW", "WINDOW_TYPE", "Borderless");
		else if (graphics.WindowType == Fullscreen)
			ini.SetValue("WINDOW", "WINDOW_TYPE", "Fullscreen");
		else //for windowed mode, used as a fallback if invalid options chosen lol
			ini.SetValue("WINDOW", "WINDOW_TYPE", "Windowed");
		ini.SetLongValue("WINDOW", "WIDTH", graphics.WindowWidth);
		ini.SetLongValue("WINDOW", "HEIGHT", graphics.WindowHeight);

		ini.SetBoolValue("DISPLAY", "V-Sync", graphics.VSync);

		ini.SetLongValue("TEXTURES", "maxAnisotropy", graphics.maxAnisotropy);

		ini.SetLongValue("GRAPHICS", "FramerateCap", graphics.FramerateCap);
		ini.SetLongValue("GRAPHICS", "MSAASamples", static_cast<int>(graphics.MSAASamples));

		ini.SetLongValue("GRAPHICS", "LODCoef", graphics.LODCoef);
		ini.SetLongValue("GRAPHICS", "ShadowQuality", graphics.ShadowQuality);
		ini.SetLongValue("GRAPHICS", "ReflectionQuality", graphics.ReflectionQuality);

		ini.SaveFile(GAME_SETTINGS_PATH);
	}
	void GraphicsSettings::setWindowMode(WindowType winType) {
		engineWindow->setWindowMode((EngineWindow::WindowType)(int)winType);
	}
	void GraphicsSettings::toggleFullscreen()
	{/*
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
		*/
		setWindowMode(Borderless);
	}
}