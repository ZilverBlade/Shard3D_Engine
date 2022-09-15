#pragma once

#include "../../utils/simple_ini.h"
#include "../rendering/window.h"
namespace Shard3D {
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

	struct RuntimeInfo {
		RuntimeInfo() = default;
		float aspectRatio = 1.f;
		glm::vec4 ambientLightColor = { 1.0f, 1.0f, 1.0f, 0.01f };;
		bool is10BitColor = false;
		glm::vec4 localScreen{1280,720, 0, 0}; // posX, posY, width, height, 

		glm::uvec3 PostProcessingInvocationIDCounts{1920 / 16, 1080 / 16, 1};
	};

	struct NonModifyableStaticInfo {
		std::string gameName = "My Shard3D Game";
	};

	struct Settings {
		Settings() = default;
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
// Amount of frames per second to limit the renderer.
// Requires V-Sync to be turned off
// 			   @param 0: uncapped
// 			   @param any other positive integer: max framerate
			//@return (live usage)
		int FramerateCap = 0;

		//	[GRAPHICS]
//Time to wait between frames, used by the renderer to reduce computation
// 			   @param 0: uncapped
// 			   @param any other positive float: time between frames
			//@return (live usage)
		const float FramerateCapInterval = 0.f;
		//	[GRAPHICS]
//Samples used by the renderer. Keep at 1, as this is deprecated and should not be used. 
			//@return (requires restart)
		VkSampleCountFlagBits MSAASamples = VK_SAMPLE_COUNT_1_BIT;
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

		glm::vec3 GlobalMaterialSettings = { 1.f, 0.5f, 0.f };

		float exposure = 1.f;
	};

	struct EditorPreviewSettings {
		EditorPreviewSettings() = default;

		bool V_EDITOR_BILLBOARDS = true;
		bool V_HIDDEN_MESHES = true;
		bool V_HIDDEN_BILLBOARDS = true;
		bool V_GRID = true;
		bool V_GUI = true;
		bool ONLY_GAME;
	};
	class GraphicsSettings {
	protected:
		friend class EngineRenderer;
	private:
		static inline Settings graphics{};
		static inline RuntimeInfo r_info{};
		static inline NonModifyableStaticInfo constInfo{};
		static inline CSimpleIniA ini;
		static inline EngineWindow* engineWindow;
	public:
		static inline EditorPreviewSettings editorPreview{};

		// you may initialise this twice, as the engine window is required later on. First initialisation can be with nullptr as argument.
		static void init(EngineWindow* window);
		static void read();
		static RuntimeInfo& getRuntimeInfo() { return r_info; }
	// as this getter passes the value by reference, you may modify its contents
		static Settings& get();

		static NonModifyableStaticInfo getStaticApplicationInfo() { return constInfo; }

		static void set(Settings sets);
		 
		static void setWindowMode(WindowType winType);
		static void toggleFullscreen();
	};

}