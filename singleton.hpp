 #pragma once

#include "device.hpp"
#include "UI/imgui_implementation.hpp"
#include "renderer.hpp"
#include "wb3d/level.hpp"
#include "offscreen.hpp"
#include "graphics_settings.hpp"
#include "audio.hpp"
#include "scripts/dynamic_script_engine.hpp"
namespace Shard3D {
	// ignore
	class HUD;
	class Initializer {
	private:
		friend class Singleton;
		Initializer() {
			LOGGER::init();
			EngineAudio::init();
			GraphicsSettings::init(nullptr);
			DynamicScriptEngine::init();
		}
	};
	class Destructor {
	private:
		friend class Singleton;
		Destructor() {}
		~Destructor();
	};
	class Singleton {
	private:
		static inline constexpr int WIDTH = 1280; //800
		static inline constexpr int HEIGHT = 720; //600
		static inline const std::string WINDOW_NAME = "Shard3D Engine"; //engine name
	public:
		Singleton() {}
		~Singleton() {}
		static inline Initializer _ignore_init;

		static inline EngineWindow engineWindow{ WIDTH, HEIGHT, WINDOW_NAME };
		static inline EngineDevice engineDevice{ engineWindow };
		static inline EngineRenderer engineRenderer{ engineWindow, engineDevice };
		static inline VkDescriptorSet viewportImage{};
		static inline OffScreen mainOffScreen{ engineDevice };

#if ALLOW_PREVIEW_CAMERA
		static inline OffScreen previewCamOffScreen{ engineDevice };
		static inline VkDescriptorSet previewViewportImage{};
#endif
		static inline std::shared_ptr<wb3d::Level> activeLevel, capturedLevel;
		static inline std::vector<HUD*> hudList;
		static inline ImGui_ImplVulkan_InitInfo imgui_init_info{};

//#ifndef _DEPLOY
		struct eps {
			bool V_EDITOR_BILLBOARDS = true;
			bool V_HIDDEN_MESHES = true;
			bool V_HIDDEN_BILLBOARDS = true;
			bool V_GRID = true;
			bool V_GUI = true;
			bool ONLY_GAME;
		};
		static inline eps editorPreviewSettings;
//#endif
		static inline glm::vec3 testPBR = {1.f, 0.5f, 0.f};

		static inline Destructor _ignore_destroy;
	};
}