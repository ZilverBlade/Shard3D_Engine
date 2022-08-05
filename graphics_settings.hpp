#pragma once

#include "simpleini/simple_ini.h"
#include "engine_window.hpp"
namespace Shard3D {
	class GraphicsSettings {
	protected:
		enum GraphicsEnum {
			Null = 0,
			Cinematic = 1,
			Ultra = 2,
			High = 3,
			Medium = 4,
			Low = 5,
			Poor = 6
		};

		enum WindowType {
			Windowed = 0,
			Borderless = 1,
			Fullscreen = 2
		};

		struct Settings {
			//	[WINDOW]
//Window width of the main window
			//@return (live usage)
			int WindowWidth = 1920;
			//	[WINDOW]
//Window height of the main window
			//@return (live usage)
			int	WindowHeight = 1000;
			//	[WINDOW]
//Window type of the main window. 
//@param Can be: Windowed, Borderless, Fullscreen.
//@return (live usage)
			WindowType WindowType = Borderless;

			//	[DISPLAY]
//Whether the FIFO present mode is used or not. When off, the swapchain will figure out to use Mailbox or Immediate
			//@return (requires swapchain recreation)
			bool VSync = true;
				
			//	[TEXTURES]
//Level of anisotropic filtering used by the texture system.
			//@return (requires restart)
			int maxAnisotropy = 16;

			//	[GRAPHICS]
//Amount of frames per second to limit the renderer.
// 			   @param -1: uncapped
// 			   @param any other positive integer: max framerate
			//@return (live usage)
			int FramerateCap = 0;
			//	[GRAPHICS]
//Samples used by the renderer. Keep at 1, as this is deprecated and should not be used. 
			//@return (requires restart)
			int	MSAASamples = 1;
			//	[GRAPHICS]
/*LOD Coef. Quality will range from graphics settings :
			@param Poor: Always lowest LODs
			@param Low: Medium LODs load in at (placeholder) distance.
			@param Medium: Medium LODs load in at (placeholder) distance, High LODs load in at (placeholder) distance.
			@param High: Medium LODs load in at (placeholder) distance, High LODs load in at (placeholder) distance.
			@param Cinematic Always highest LODs
			@return (live usage)
						*/
			GraphicsEnum LODCoef = Medium;
			//	[GRAPHICS]
/*Shadowmap quality. Quality will range from graphics settings :
 			@param Poor: No shadows
 			@param  Low: (placeholder)
 			@param  Medium: (placeholder)
 			@param  High: (placeholder)
 			@param  Ultra: (placeholder) + (placeholder)
 			@param  Cinematic: (placeholder) + (placeholder) (should not be used for real-time rendering)
			@return (live usage)
			*/
			GraphicsEnum ShadowQuality = Medium;
			//	[GRAPHICS]
//placeholder
			//@param Can be: Low, Medium, High
			//@return (live usage)
			GraphicsEnum ReflectionQuality = Medium;
		};
	
	private:
		inline static Settings graphics;
		inline static CSimpleIniA ini;
		inline static EngineWindow* engineWindow;
	public:
		// you may initialise this twice, as the engine window is required later on. First initialisation can be with nullptr as argument.
		static void init(EngineWindow* window);
		static void read();
		// as this getter passes the value by reference, you may modify its contents
		static Settings& get();
		static void set(Settings sets);
		 
		static void setWindowMode(WindowType winType);
		static void toggleFullscreen();
	};

}